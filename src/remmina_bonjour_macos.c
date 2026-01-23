/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2024-2024 The Remmina Project
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

#ifdef __APPLE__

#include "remmina_bonjour_macos.h"
#include <dns_sd.h>
#include <arpa/inet.h>
#include <string.h>

/* Structure to hold browse operation context */
typedef struct {
	DNSServiceRef service_ref;
	RemminaServiceCallback callback;
	gpointer user_data;
	guint watch_id;
} BrowseContext;

/* Structure to hold resolve operation context */
typedef struct {
	RemminaServiceCallback callback;
	gpointer user_data;
	DNSServiceRef service_ref;
	guint watch_id;
} ResolveContext;

/* Global list of active browse contexts */
static GSList *browse_contexts = NULL;

/* Forward declarations */
static void cleanup_browse_context(BrowseContext *ctx);
static void cleanup_resolve_context(ResolveContext *ctx);

/**
 * dns_sd_callback_handler:
 *
 * GLib I/O watch callback that processes DNS-SD events.
 */
static gboolean
dns_sd_callback_handler(GIOChannel *source, GIOCondition condition, gpointer data)
{
	DNSServiceRef service_ref = (DNSServiceRef)data;
	
	if (condition & G_IO_IN) {
		DNSServiceErrorType err = DNSServiceProcessResult(service_ref);
		if (err != kDNSServiceErr_NoError) {
			g_warning("DNSServiceProcessResult failed with error %d", err);
			return FALSE;
		}
		return TRUE;
	}
	
	return FALSE;
}

/**
 * resolve_callback:
 *
 * Callback invoked when a service is resolved.
 */
static void DNSSD_API
resolve_callback(DNSServiceRef sdRef,
		DNSServiceFlags flags,
		uint32_t interfaceIndex,
		DNSServiceErrorType errorCode,
		const char *fullname,
		const char *hosttarget,
		uint16_t port,
		uint16_t txtLen,
		const unsigned char *txtRecord,
		void *context)
{
	ResolveContext *ctx = (ResolveContext *)context;
	
	if (errorCode != kDNSServiceErr_NoError) {
		g_warning("Service resolution failed with error %d", errorCode);
		cleanup_resolve_context(ctx);
		return;
	}
	
	/* Convert port from network byte order */
	uint16_t host_port = ntohs(port);
	
	/* Extract service name from fullname (remove domain suffix) */
	gchar *service_name = g_strdup(fullname);
	gchar *dot = strchr(service_name, '.');
	if (dot) {
		*dot = '\0';
	}
	
	/* Invoke the user callback */
	if (ctx->callback) {
		ctx->callback(service_name, hosttarget, host_port, ctx->user_data);
	}
	
	g_free(service_name);
	
	/* Clean up resolve context */
	cleanup_resolve_context(ctx);
}

/**
 * browse_callback:
 *
 * Callback invoked when services are discovered or removed.
 */
static void DNSSD_API
browse_callback(DNSServiceRef sdRef,
		DNSServiceFlags flags,
		uint32_t interfaceIndex,
		DNSServiceErrorType errorCode,
		const char *serviceName,
		const char *regtype,
		const char *replyDomain,
		void *context)
{
	BrowseContext *ctx = (BrowseContext *)context;
	
	if (errorCode != kDNSServiceErr_NoError) {
		g_warning("Service browsing failed with error %d", errorCode);
		return;
	}
	
	/* Only handle service additions */
	if (!(flags & kDNSServiceFlagsAdd)) {
		return;
	}
	
	/* Create resolve context */
	ResolveContext *resolve_ctx = g_new0(ResolveContext, 1);
	resolve_ctx->callback = ctx->callback;
	resolve_ctx->user_data = ctx->user_data;
	
	/* Start resolving the service */
	DNSServiceErrorType err = DNSServiceResolve(
		&resolve_ctx->service_ref,
		0,
		interfaceIndex,
		serviceName,
		regtype,
		replyDomain,
		resolve_callback,
		resolve_ctx);
	
	if (err != kDNSServiceErr_NoError) {
		g_warning("DNSServiceResolve failed with error %d", err);
		g_free(resolve_ctx);
		return;
	}
	
	/* Integrate with GLib main loop */
	int fd = DNSServiceRefSockFD(resolve_ctx->service_ref);
	if (fd == -1) {
		g_warning("DNSServiceRefSockFD failed");
		DNSServiceRefDeallocate(resolve_ctx->service_ref);
		g_free(resolve_ctx);
		return;
	}
	
	GIOChannel *channel = g_io_channel_unix_new(fd);
	resolve_ctx->watch_id = g_io_add_watch(channel,
		G_IO_IN,
		dns_sd_callback_handler,
		resolve_ctx->service_ref);
	g_io_channel_unref(channel);
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
	
	if (ctx->watch_id > 0) {
		g_source_remove(ctx->watch_id);
		ctx->watch_id = 0;
	}
	
	if (ctx->service_ref) {
		DNSServiceRefDeallocate(ctx->service_ref);
		ctx->service_ref = NULL;
	}
	
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
	
	if (ctx->watch_id > 0) {
		g_source_remove(ctx->watch_id);
		ctx->watch_id = 0;
	}
	
	if (ctx->service_ref) {
		DNSServiceRefDeallocate(ctx->service_ref);
		ctx->service_ref = NULL;
	}
	
	g_free(ctx);
}

/**
 * remmina_service_discovery_init:
 *
 * Initialize the Bonjour service discovery system.
 */
gboolean
remmina_service_discovery_init(void)
{
	/* No initialization needed for Bonjour */
	return TRUE;
}

/**
 * remmina_service_discovery_browse:
 *
 * Start browsing for network services of the specified type.
 */
gboolean
remmina_service_discovery_browse(const gchar *service_type,
				RemminaServiceCallback callback,
				gpointer user_data)
{
	if (!service_type || !callback) {
		g_warning("Invalid parameters for service discovery");
		return FALSE;
	}
	
	/* Create browse context */
	BrowseContext *ctx = g_new0(BrowseContext, 1);
	ctx->callback = callback;
	ctx->user_data = user_data;
	
	/* Start browsing */
	DNSServiceErrorType err = DNSServiceBrowse(
		&ctx->service_ref,
		0,
		0,
		service_type,
		NULL,
		browse_callback,
		ctx);
	
	if (err != kDNSServiceErr_NoError) {
		g_warning("DNSServiceBrowse failed with error %d", err);
		g_free(ctx);
		return FALSE;
	}
	
	/* Integrate with GLib main loop */
	int fd = DNSServiceRefSockFD(ctx->service_ref);
	if (fd == -1) {
		g_warning("DNSServiceRefSockFD failed");
		DNSServiceRefDeallocate(ctx->service_ref);
		g_free(ctx);
		return FALSE;
	}
	
	GIOChannel *channel = g_io_channel_unix_new(fd);
	ctx->watch_id = g_io_add_watch(channel,
		G_IO_IN,
		dns_sd_callback_handler,
		ctx->service_ref);
	g_io_channel_unref(channel);
	
	/* Add to global list */
	browse_contexts = g_slist_append(browse_contexts, ctx);
	
	return TRUE;
}

/**
 * remmina_service_discovery_stop:
 *
 * Stop all active service discovery operations.
 */
void
remmina_service_discovery_stop(void)
{
	GSList *iter;
	
	for (iter = browse_contexts; iter != NULL; iter = iter->next) {
		BrowseContext *ctx = (BrowseContext *)iter->data;
		cleanup_browse_context(ctx);
	}
	
	g_slist_free(browse_contexts);
	browse_contexts = NULL;
}

#endif /* __APPLE__ */
