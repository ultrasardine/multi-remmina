/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2009-2011 Vic Lee
 * Copyright (C) 2014-2015 Antenore Gatta, Fabio Castelli, Giovanni Panozzo
 * Copyright (C) 2016-2023 Antenore Gatta, Giovanni Panozzo
 * Copyright (C) 2023-2024 Hiroyuki Tanaka, Sunil Bhat
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of portions of this program with the
 *  OpenSSL library under certain conditions as described in each
 *  individual source file, and distribute linked combinations
 *  including the two.
 *  You must obey the GNU General Public License in all respects
 *  for all of the code used other than OpenSSL. *  If you modify
 *  file(s) with this exception, you may extend this exception to your
 *  version of the file(s), but you are not obligated to do so. *  If you
 *  do not wish to do so, delete this exception statement from your
 *  version. *  If you delete this exception statement from all source
 *  files in the program, then also delete it here.
 *
 */

#ifdef _WIN32

#include "remmina_dnssd_windows.h"
#include <windows.h>
#include <windns.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

/* Link with dnsapi.lib and ws2_32.lib */
#pragma comment(lib, "dnsapi.lib")
#pragma comment(lib, "ws2_32.lib")

/* Structure to hold browse operation context */
typedef struct {
	RemminaDnssdCallback callback;
	gpointer user_data;
	gchar *service_type;
	DNS_SERVICE_BROWSE_REQUEST browse_request;
	DNS_SERVICE_CANCEL cancel_handle;
	guint timeout_id;
	gboolean active;
} BrowseContext;

/* Structure to hold resolve operation context */
typedef struct {
	RemminaDnssdCallback callback;
	gpointer user_data;
	gchar *service_name;
	DNS_SERVICE_RESOLVE_REQUEST resolve_request;
	DNS_SERVICE_CANCEL cancel_handle;
	guint timeout_id;
} ResolveContext;

/* Global state */
static GSList *browse_contexts = NULL;
static gboolean initialized = FALSE;
static GMutex dnssd_mutex;

/* Forward declarations */
static void cleanup_browse_context(BrowseContext *ctx);
static void cleanup_resolve_context(ResolveContext *ctx);
static void WINAPI browse_callback(DWORD status, PVOID context, PDNS_RECORD records);
static void WINAPI resolve_callback(DWORD status, PVOID context, PDNS_SERVICE_INSTANCE instance);


/**
 * Convert UTF-8 string to wide character string (UTF-16).
 * Returns newly allocated wide string that must be freed with g_free.
 */
static wchar_t* utf8_to_wide(const gchar *utf8_str)
{
	if (!utf8_str)
		return NULL;
	
	glong items_written;
	gunichar2 *utf16 = g_utf8_to_utf16(utf8_str, -1, NULL, &items_written, NULL);
	
	return (wchar_t*)utf16;
}

/**
 * Convert wide character string (UTF-16) to UTF-8 string.
 * Returns newly allocated UTF-8 string that must be freed with g_free.
 */
static gchar* wide_to_utf8(const wchar_t *wide_str)
{
	if (!wide_str)
		return NULL;
	
	return g_utf16_to_utf8((const gunichar2*)wide_str, -1, NULL, NULL, NULL);
}

/**
 * Process browse results in the main thread.
 * This is called via g_idle_add to ensure thread safety with GTK.
 */
typedef struct {
	RemminaDnssdCallback callback;
	gpointer user_data;
	gchar *service_name;
	gchar *hostname;
	guint16 port;
} ServiceResult;

static gboolean
process_service_result(gpointer data)
{
	ServiceResult *result = (ServiceResult *)data;
	
	if (result->callback) {
		result->callback(result->service_name, result->hostname, 
				 result->port, result->user_data);
	}
	
	g_free(result->service_name);
	g_free(result->hostname);
	g_free(result);
	
	return G_SOURCE_REMOVE;
}

/**
 * resolve_callback:
 *
 * Callback invoked when a service is resolved.
 * Called from Windows DNS-SD thread, so we dispatch to main thread.
 */
static void WINAPI
resolve_callback(DWORD status, PVOID context, PDNS_SERVICE_INSTANCE instance)
{
	ResolveContext *ctx = (ResolveContext *)context;
	
	if (status != ERROR_SUCCESS) {
		g_warning("DNS-SD service resolution failed with error %lu", (unsigned long)status);
		cleanup_resolve_context(ctx);
		return;
	}
	
	if (!instance) {
		g_warning("DNS-SD service resolution returned NULL instance");
		cleanup_resolve_context(ctx);
		return;
	}
	
	/* Extract service information */
	gchar *hostname = NULL;
	guint16 port = 0;
	
	if (instance->pszHostName) {
		hostname = wide_to_utf8(instance->pszHostName);
	}
	
	port = instance->wPort;
	
	/* Dispatch result to main thread */
	if (ctx->callback && hostname) {
		ServiceResult *result = g_new0(ServiceResult, 1);
		result->callback = ctx->callback;
		result->user_data = ctx->user_data;
		result->service_name = g_strdup(ctx->service_name);
		result->hostname = hostname;
		result->port = port;
		
		g_idle_add(process_service_result, result);
	} else {
		g_free(hostname);
	}
	
	/* Free the instance returned by Windows */
	DnsServiceFreeInstance(instance);
	
	/* Clean up resolve context */
	cleanup_resolve_context(ctx);
}


/**
 * Start resolving a discovered service.
 */
static void
start_resolve(BrowseContext *browse_ctx, const gchar *service_name)
{
	if (!browse_ctx || !service_name)
		return;
	
	/* Create resolve context */
	ResolveContext *ctx = g_new0(ResolveContext, 1);
	ctx->callback = browse_ctx->callback;
	ctx->user_data = browse_ctx->user_data;
	ctx->service_name = g_strdup(service_name);
	
	/* Build the full service name for resolution */
	gchar *full_name = g_strdup_printf("%s.%s.local", service_name, browse_ctx->service_type);
	wchar_t *wide_name = utf8_to_wide(full_name);
	g_free(full_name);
	
	if (!wide_name) {
		g_warning("Failed to convert service name to wide string");
		cleanup_resolve_context(ctx);
		return;
	}
	
	/* Initialize resolve request */
	memset(&ctx->resolve_request, 0, sizeof(DNS_SERVICE_RESOLVE_REQUEST));
	ctx->resolve_request.Version = DNS_QUERY_REQUEST_VERSION1;
	ctx->resolve_request.pszServiceName = wide_name;
	ctx->resolve_request.pResolveCompletionCallback = resolve_callback;
	ctx->resolve_request.pQueryContext = ctx;
	
	/* Start the resolve operation */
	DNS_STATUS dns_status = DnsServiceResolve(&ctx->resolve_request, &ctx->cancel_handle);
	
	if (dns_status != DNS_REQUEST_PENDING && dns_status != ERROR_SUCCESS) {
		g_warning("DnsServiceResolve failed with error %lu", (unsigned long)dns_status);
		g_free(wide_name);
		cleanup_resolve_context(ctx);
		return;
	}
	
	/* Note: wide_name is used by the resolve request, so we don't free it here.
	 * It will be freed when the resolve context is cleaned up. */
}

/**
 * browse_callback:
 *
 * Callback invoked when services are discovered.
 * Called from Windows DNS-SD thread.
 */
static void WINAPI
browse_callback(DWORD status, PVOID context, PDNS_RECORD records)
{
	BrowseContext *ctx = (BrowseContext *)context;
	
	if (!ctx || !ctx->active)
		return;
	
	if (status != ERROR_SUCCESS) {
		g_warning("DNS-SD browse failed with error %lu", (unsigned long)status);
		return;
	}
	
	/* Process discovered services */
	PDNS_RECORD current = records;
	while (current) {
		if (current->wType == DNS_TYPE_PTR && current->Data.PTR.pNameHost) {
			gchar *service_name = wide_to_utf8(current->Data.PTR.pNameHost);
			if (service_name) {
				/* Extract just the service instance name (before the service type) */
				gchar *dot = strchr(service_name, '.');
				if (dot) {
					*dot = '\0';
				}
				
				g_print("(remmina-applet dnssd) Found service '%s' of type '%s'\n", 
					service_name, ctx->service_type);
				
				/* Start resolving this service */
				start_resolve(ctx, service_name);
				
				g_free(service_name);
			}
		}
		current = current->pNext;
	}
	
	/* Free the records returned by Windows */
	if (records) {
		DnsRecordListFree(records, DnsFreeRecordList);
	}
}


/**
 * cleanup_browse_context:
 *
 * Clean up a browse context and free resources.
 */
static void
cleanup_browse_context(BrowseContext *ctx)
{
	if (!ctx)
		return;
	
	ctx->active = FALSE;
	
	if (ctx->timeout_id > 0) {
		g_source_remove(ctx->timeout_id);
		ctx->timeout_id = 0;
	}
	
	/* Cancel the browse operation if active */
	if (ctx->cancel_handle.reserved) {
		DnsServiceBrowseCancel(&ctx->cancel_handle);
		memset(&ctx->cancel_handle, 0, sizeof(DNS_SERVICE_CANCEL));
	}
	
	g_free(ctx->service_type);
	g_free(ctx);
}

/**
 * cleanup_resolve_context:
 *
 * Clean up a resolve context and free resources.
 */
static void
cleanup_resolve_context(ResolveContext *ctx)
{
	if (!ctx)
		return;
	
	if (ctx->timeout_id > 0) {
		g_source_remove(ctx->timeout_id);
		ctx->timeout_id = 0;
	}
	
	/* Cancel the resolve operation if active */
	if (ctx->cancel_handle.reserved) {
		DnsServiceResolveCancel(&ctx->cancel_handle);
		memset(&ctx->cancel_handle, 0, sizeof(DNS_SERVICE_CANCEL));
	}
	
	/* Free the service name stored in the resolve request */
	if (ctx->resolve_request.pszServiceName) {
		g_free((gpointer)ctx->resolve_request.pszServiceName);
	}
	
	g_free(ctx->service_name);
	g_free(ctx);
}

/**
 * remmina_dnssd_windows_init:
 *
 * Initialize the Windows DNS-SD service discovery system.
 */
gboolean
remmina_dnssd_windows_init(void)
{
	if (initialized)
		return TRUE;
	
	g_mutex_init(&dnssd_mutex);
	
	/* Initialize Winsock (required for DNS operations) */
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0) {
		g_warning("WSAStartup failed with error %d", result);
		return FALSE;
	}
	
	initialized = TRUE;
	g_print("(remmina-applet dnssd) Windows DNS-SD initialized\n");
	
	return TRUE;
}

/**
 * remmina_dnssd_windows_browse:
 *
 * Start browsing for network services of the specified type.
 */
gboolean
remmina_dnssd_windows_browse(const gchar *service_type,
			      RemminaDnssdCallback callback,
			      gpointer user_data)
{
	if (!initialized) {
		g_warning("DNS-SD not initialized. Call remmina_dnssd_windows_init() first.");
		return FALSE;
	}
	
	if (!service_type || !callback) {
		g_warning("Invalid parameters for DNS-SD browse");
		return FALSE;
	}
	
	g_mutex_lock(&dnssd_mutex);
	
	/* Create browse context */
	BrowseContext *ctx = g_new0(BrowseContext, 1);
	ctx->callback = callback;
	ctx->user_data = user_data;
	ctx->service_type = g_strdup(service_type);
	ctx->active = TRUE;
	
	/* Build the browse query name (e.g., "_rfb._tcp.local") */
	gchar *query_name = g_strdup_printf("%s.local", service_type);
	wchar_t *wide_query = utf8_to_wide(query_name);
	g_free(query_name);
	
	if (!wide_query) {
		g_warning("Failed to convert service type to wide string");
		cleanup_browse_context(ctx);
		g_mutex_unlock(&dnssd_mutex);
		return FALSE;
	}
	
	/* Initialize browse request */
	memset(&ctx->browse_request, 0, sizeof(DNS_SERVICE_BROWSE_REQUEST));
	ctx->browse_request.Version = DNS_QUERY_REQUEST_VERSION1;
	ctx->browse_request.InterfaceIndex = 0; /* All interfaces */
	ctx->browse_request.pszServiceName = wide_query;
	ctx->browse_request.pBrowseCallback = browse_callback;
	ctx->browse_request.pQueryContext = ctx;
	
	/* Start the browse operation */
	DNS_STATUS dns_status = DnsServiceBrowse(&ctx->browse_request, &ctx->cancel_handle);
	
	if (dns_status != DNS_REQUEST_PENDING && dns_status != ERROR_SUCCESS) {
		g_warning("DnsServiceBrowse failed with error %lu for service type '%s'", 
			  (unsigned long)dns_status, service_type);
		g_free(wide_query);
		cleanup_browse_context(ctx);
		g_mutex_unlock(&dnssd_mutex);
		return FALSE;
	}
	
	/* Add to global list */
	browse_contexts = g_slist_append(browse_contexts, ctx);
	
	g_print("(remmina-applet dnssd) Started browsing for '%s'\n", service_type);
	
	g_mutex_unlock(&dnssd_mutex);
	
	/* Note: wide_query is used by the browse request, so we don't free it here.
	 * It will be freed when the browse context is cleaned up. */
	
	return TRUE;
}


/**
 * remmina_dnssd_windows_stop:
 *
 * Stop all active service discovery operations.
 */
void
remmina_dnssd_windows_stop(void)
{
	if (!initialized)
		return;
	
	g_mutex_lock(&dnssd_mutex);
	
	GSList *iter;
	for (iter = browse_contexts; iter != NULL; iter = iter->next) {
		BrowseContext *ctx = (BrowseContext *)iter->data;
		cleanup_browse_context(ctx);
	}
	
	g_slist_free(browse_contexts);
	browse_contexts = NULL;
	
	g_print("(remmina-applet dnssd) Stopped all service discovery\n");
	
	g_mutex_unlock(&dnssd_mutex);
}

/**
 * remmina_dnssd_windows_cleanup:
 *
 * Clean up all DNS-SD resources.
 */
void
remmina_dnssd_windows_cleanup(void)
{
	if (!initialized)
		return;
	
	remmina_dnssd_windows_stop();
	
	/* Clean up Winsock */
	WSACleanup();
	
	g_mutex_clear(&dnssd_mutex);
	
	initialized = FALSE;
	
	g_print("(remmina-applet dnssd) Windows DNS-SD cleaned up\n");
}

/**
 * remmina_dnssd_windows_is_available:
 *
 * Check if DNS-SD service discovery is available on this system.
 * On Windows 10 version 1803 and later, DNS-SD APIs are available.
 */
gboolean
remmina_dnssd_windows_is_available(void)
{
	/* Check Windows version - DNS-SD APIs require Windows 10 1803+ */
	OSVERSIONINFOEXW osvi;
	DWORDLONG condition_mask = 0;
	
	memset(&osvi, 0, sizeof(OSVERSIONINFOEXW));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	osvi.dwMajorVersion = 10;
	osvi.dwMinorVersion = 0;
	osvi.dwBuildNumber = 17134; /* Windows 10 version 1803 */
	
	VER_SET_CONDITION(condition_mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(condition_mask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(condition_mask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	
	if (!VerifyVersionInfoW(&osvi, 
				VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER,
				condition_mask)) {
		g_warning("DNS-SD APIs require Windows 10 version 1803 or later");
		return FALSE;
	}
	
	/* Try to load dnsapi.dll and check for required functions */
	HMODULE dnsapi = LoadLibraryW(L"dnsapi.dll");
	if (!dnsapi) {
		g_warning("Failed to load dnsapi.dll");
		return FALSE;
	}
	
	/* Check for DnsServiceBrowse function */
	FARPROC browse_func = GetProcAddress(dnsapi, "DnsServiceBrowse");
	FARPROC resolve_func = GetProcAddress(dnsapi, "DnsServiceResolve");
	
	FreeLibrary(dnsapi);
	
	if (!browse_func || !resolve_func) {
		g_warning("DNS-SD functions not available in dnsapi.dll");
		return FALSE;
	}
	
	return TRUE;
}

#endif /* _WIN32 */
