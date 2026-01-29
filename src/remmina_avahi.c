/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2009-2010 Vic Lee
 * Copyright (C) 2014-2015 Antenore Gatta, Fabio Castelli, Giovanni Panozzo
 * Copyright (C) 2016-2023 Antenore Gatta, Giovanni Panozzo
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

#include "config.h"
#include "remmina_avahi.h"
#include "remmina/remmina_trace_calls.h"

#ifdef _WIN32
/* Windows uses DNS-SD APIs from dnsapi.dll */
#include "remmina_dnssd_windows.h"

/* Structure to hold service discovery state for Windows */
typedef struct {
	RemminaAvahi *ga;
	gchar *service_type;
} WindowsServiceContext;

/* Free service context */
static void
windows_service_context_free(WindowsServiceContext *ctx)
{
	if (ctx) {
		g_free(ctx->service_type);
		g_free(ctx);
	}
}

/* Callback adapter to convert DNS-SD callback to Avahi-style hash table */
static void
windows_service_callback_adapter(const gchar *service_name,
				  const gchar *hostname,
				  guint16 port,
				  gpointer user_data)
{
	WindowsServiceContext *ctx = (WindowsServiceContext *)user_data;
	RemminaAvahi *ga = ctx->ga;
	gchar *key;
	gchar *value;

	/* Create key in same format as Avahi: "name,type,domain" */
	key = g_strdup_printf("%s,%s,local", service_name, ctx->service_type);
	
	/* Check if already in hash table */
	if (g_hash_table_lookup(ga->discovered_services, key)) {
		g_free(key);
		return;
	}

	/* Create value in same format as Avahi: "[hostname]:port" */
	value = g_strdup_printf("[%s]:%i", hostname, port);
	g_hash_table_insert(ga->discovered_services, key, value);
	/* key and value will be freed with g_free when the hash table is freed */

	g_print("(remmina-applet dnssd) Added service '%s' (%s)\n", value, ctx->service_type);
}

RemminaAvahi* remmina_avahi_new(void)
{
	TRACE_CALL(__func__);
	RemminaAvahi* ga;

	ga = g_new0(RemminaAvahi, 1);
	ga->discovered_services = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	ga->started = FALSE;
	/* Use priv to store list of service contexts for cleanup */
	ga->priv = NULL;

	return ga;
}

void remmina_avahi_start(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	WindowsServiceContext *ctx;
	GSList *contexts = NULL;
	const gchar *service_types[] = { "_rfb._tcp", "_rdp._tcp", "_ssh._tcp", NULL };
	gint i;

	if (ga->started)
		return;

	ga->started = TRUE;

	/* Check if DNS-SD is available on this Windows version */
	if (!remmina_dnssd_windows_is_available()) {
		g_warning("DNS-SD service discovery not available on this Windows version");
		return;
	}

	/* Initialize Windows DNS-SD service discovery */
	if (!remmina_dnssd_windows_init()) {
		g_warning("Failed to initialize Windows DNS-SD service discovery");
		return;
	}

	/* Start browsing for multiple service types */
	for (i = 0; service_types[i] != NULL; i++) {
		ctx = g_new0(WindowsServiceContext, 1);
		ctx->ga = ga;
		ctx->service_type = g_strdup(service_types[i]);
		
		if (!remmina_dnssd_windows_browse(service_types[i], 
						   windows_service_callback_adapter, 
						   ctx)) {
			g_warning("Failed to start DNS-SD browsing for %s", service_types[i]);
			windows_service_context_free(ctx);
			continue;
		}
		
		/* Store context for cleanup */
		contexts = g_slist_append(contexts, ctx);
	}

	/* Store contexts list in priv for cleanup */
	ga->priv = (RemminaAvahiPriv*)contexts;

	g_print("(remmina-applet dnssd) Started service discovery\n");
}

void remmina_avahi_stop(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	GSList *contexts;
	
	if (!ga->started)
		return;

	g_hash_table_remove_all(ga->discovered_services);
	
	/* Stop Windows DNS-SD service discovery */
	remmina_dnssd_windows_stop();
	
	/* Free all service contexts */
	contexts = (GSList *)ga->priv;
	g_slist_free_full(contexts, (GDestroyNotify)windows_service_context_free);
	ga->priv = NULL;
	
	ga->started = FALSE;
}

void remmina_avahi_free(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	if (ga == NULL)
		return;

	remmina_avahi_stop(ga);
	
	/* Cleanup Windows DNS-SD resources */
	remmina_dnssd_windows_cleanup();

	g_hash_table_destroy(ga->discovered_services);
	g_free(ga);
}

#elif defined(__APPLE__)
/* macOS uses Bonjour instead of Avahi */
#include "remmina_bonjour_macos.h"

/* Structure to hold service discovery state for macOS */
typedef struct {
	RemminaAvahi *ga;
	gchar *service_type;
} MacOSServiceContext;

/* Free service context */
static void
macos_service_context_free(MacOSServiceContext *ctx)
{
	if (ctx) {
		g_free(ctx->service_type);
		g_free(ctx);
	}
}

/* Callback adapter to convert Bonjour callback to Avahi-style hash table */
static void
macos_service_callback_adapter(const gchar *service_name,
				const gchar *hostname,
				guint16 port,
				gpointer user_data)
{
	MacOSServiceContext *ctx = (MacOSServiceContext *)user_data;
	RemminaAvahi *ga = ctx->ga;
	gchar *key;
	gchar *value;

	/* Create key in same format as Avahi: "name,type,domain" */
	key = g_strdup_printf("%s,%s,local", service_name, ctx->service_type);
	
	/* Check if already in hash table */
	if (g_hash_table_lookup(ga->discovered_services, key)) {
		g_free(key);
		return;
	}

	/* Create value in same format as Avahi: "[hostname]:port" */
	value = g_strdup_printf("[%s]:%i", hostname, port);
	g_hash_table_insert(ga->discovered_services, key, value);
	/* key and value will be freed with g_free when the hash table is freed */

	g_print("(remmina-applet bonjour) Added service '%s' (%s)\n", value, ctx->service_type);
}

RemminaAvahi* remmina_avahi_new(void)
{
	TRACE_CALL(__func__);
	RemminaAvahi* ga;

	ga = g_new0(RemminaAvahi, 1);
	ga->discovered_services = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	ga->started = FALSE;
	/* Use priv to store list of service contexts for cleanup */
	ga->priv = NULL;

	return ga;
}

void remmina_avahi_start(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	MacOSServiceContext *ctx;
	GSList *contexts = NULL;
	const gchar *service_types[] = { "_rfb._tcp", "_rdp._tcp", "_ssh._tcp", NULL };
	gint i;

	if (ga->started)
		return;

	ga->started = TRUE;

	/* Initialize Bonjour service discovery */
	if (!remmina_service_discovery_init()) {
		g_warning("Failed to initialize Bonjour service discovery");
		return;
	}

	/* Start browsing for multiple service types */
	for (i = 0; service_types[i] != NULL; i++) {
		ctx = g_new0(MacOSServiceContext, 1);
		ctx->ga = ga;
		ctx->service_type = g_strdup(service_types[i]);
		
		if (!remmina_service_discovery_browse(service_types[i], 
						       macos_service_callback_adapter, 
						       ctx)) {
			g_warning("Failed to start Bonjour browsing for %s", service_types[i]);
			macos_service_context_free(ctx);
			continue;
		}
		
		/* Store context for cleanup */
		contexts = g_slist_append(contexts, ctx);
	}

	/* Store contexts list in priv for cleanup */
	ga->priv = (RemminaAvahiPriv*)contexts;

	g_print("(remmina-applet bonjour) Started service discovery\n");
}

void remmina_avahi_stop(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	GSList *contexts;
	
	if (!ga->started)
		return;

	g_hash_table_remove_all(ga->discovered_services);
	
	/* Stop Bonjour service discovery */
	remmina_service_discovery_stop();
	
	/* Free all service contexts */
	contexts = (GSList *)ga->priv;
	g_slist_free_full(contexts, (GDestroyNotify)macos_service_context_free);
	ga->priv = NULL;
	
	ga->started = FALSE;
}

void remmina_avahi_free(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	if (ga == NULL)
		return;

	remmina_avahi_stop(ga);

	g_hash_table_destroy(ga->discovered_services);
	g_free(ga);
}

#elif defined(HAVE_LIBAVAHI_CLIENT)
/* Linux/Unix uses Avahi */

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

struct _RemminaAvahiPriv {
	AvahiSimplePoll* simple_poll;
	AvahiClient* client;
	AvahiServiceBrowser* sb;
	guint iterate_handler;
	gboolean has_event;
};

static void
remmina_avahi_resolve_callback(
	AvahiServiceResolver* r,
	AVAHI_GCC_UNUSED AvahiIfIndex interface,
	AVAHI_GCC_UNUSED AvahiProtocol protocol,
	AvahiResolverEvent event,
	const char* name,
	const char* type,
	const char* domain,
	const char* host_name,
	const AvahiAddress* address,
	uint16_t port,
	AvahiStringList* txt,
	AvahiLookupResultFlags flags,
	AVAHI_GCC_UNUSED void* userdata)
{
	TRACE_CALL(__func__);
	gchar* key;
	gchar* value;
	RemminaAvahi* ga = (RemminaAvahi*)userdata;

	assert(r);

	ga->priv->has_event = TRUE;

	switch (event) {
	case AVAHI_RESOLVER_FAILURE:
		g_print("(remmina-applet avahi-resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n",
			name, type, domain, avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
		break;

	case AVAHI_RESOLVER_FOUND:
		key = g_strdup_printf("%s,%s,%s", name, type, domain);
		if (g_hash_table_lookup(ga->discovered_services, key)) {
			g_free(key);
			break;
		}
		value = g_strdup_printf("[%s]:%i", host_name, port);
		g_hash_table_insert(ga->discovered_services, key, value);
		/* key and value will be freed with g_free when the has table is freed */

		g_print("(remmina-applet avahi-resolver) Added service '%s'\n", value);

		break;
	}

	avahi_service_resolver_free(r);
}

static void
remmina_avahi_browse_callback(
	AvahiServiceBrowser* b,
	AvahiIfIndex interface,
	AvahiProtocol protocol,
	AvahiBrowserEvent event,
	const char* name,
	const char* type,
	const char* domain,
	AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
	void* userdata)
{
	TRACE_CALL(__func__);
	gchar* key;
	RemminaAvahi* ga = (RemminaAvahi*)userdata;

	assert(b);

	ga->priv->has_event = TRUE;

	switch (event) {
	case AVAHI_BROWSER_FAILURE:
		g_print("(remmina-applet avahi-browser) %s\n",
			avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))));
		return;

	case AVAHI_BROWSER_NEW:
		key = g_strdup_printf("%s,%s,%s", name, type, domain);
		if (g_hash_table_lookup(ga->discovered_services, key)) {
			g_free(key);
			break;
		}
		g_free(key);

		g_print("(remmina-applet avahi-browser) Found service '%s' of type '%s' in domain '%s'\n", name, type, domain);

		if (!(avahi_service_resolver_new(ga->priv->client, interface, protocol, name, type, domain,
			      AVAHI_PROTO_UNSPEC, 0, remmina_avahi_resolve_callback, ga))) {
			g_print("(remmina-applet avahi-browser) Failed to resolve service '%s': %s\n",
				name, avahi_strerror(avahi_client_errno(ga->priv->client)));
		}
		break;

	case AVAHI_BROWSER_REMOVE:
		g_print("(remmina-applet avahi-browser) Removed service '%s' of type '%s' in domain '%s'\n", name, type, domain);
		key = g_strdup_printf("%s,%s,%s", name, type, domain);
		g_hash_table_remove(ga->discovered_services, key);
		g_free(key);
		break;

	case AVAHI_BROWSER_ALL_FOR_NOW:
	case AVAHI_BROWSER_CACHE_EXHAUSTED:
		break;
	}
}

static void remmina_avahi_client_callback(AvahiClient* c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata)
{
	TRACE_CALL(__func__);
	RemminaAvahi* ga = (RemminaAvahi*)userdata;

	ga->priv->has_event = TRUE;

	if (state == AVAHI_CLIENT_FAILURE) {
		g_print("(remmina-applet avahi) Server connection failure: %s\n", avahi_strerror(avahi_client_errno(c)));
	}
}

static gboolean remmina_avahi_iterate(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	while (TRUE) {
		/* Call the iteration until no further events */
		ga->priv->has_event = FALSE;
		avahi_simple_poll_iterate(ga->priv->simple_poll, 0);
		if (!ga->priv->has_event)
			break;
	}

	return TRUE;
}

RemminaAvahi* remmina_avahi_new(void)
{
	TRACE_CALL(__func__);
	RemminaAvahi* ga;

	ga = g_new(RemminaAvahi, 1);
	ga->discovered_services = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	ga->started = FALSE;
	ga->priv = g_new(RemminaAvahiPriv, 1);
	ga->priv->simple_poll = NULL;
	ga->priv->client = NULL;
	ga->priv->sb = NULL;
	ga->priv->iterate_handler = 0;
	ga->priv->has_event = FALSE;

	return ga;
}

void remmina_avahi_start(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	int error;

	if (ga->started)
		return;

	ga->started = TRUE;

	ga->priv->simple_poll = avahi_simple_poll_new();
	if (!ga->priv->simple_poll) {
		g_print("Failed to create simple poll object.\n");
		return;
	}

	ga->priv->client = avahi_client_new(avahi_simple_poll_get(ga->priv->simple_poll), 0, remmina_avahi_client_callback, ga,
		&error);
	if (!ga->priv->client) {
		g_print("Failed to create client: %s\n", avahi_strerror(error));
		return;
	}

	/** @todo Customize the default domain here */
	ga->priv->sb = avahi_service_browser_new(ga->priv->client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_rfb._tcp", NULL, 0,
		remmina_avahi_browse_callback, ga);
	if (!ga->priv->sb) {
		g_print("Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(ga->priv->client)));
		return;
	}

	ga->priv->iterate_handler = g_timeout_add(5000, (GSourceFunc)remmina_avahi_iterate, ga);
}

void remmina_avahi_stop(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	g_hash_table_remove_all(ga->discovered_services);
	if (ga->priv->iterate_handler) {
		g_source_remove(ga->priv->iterate_handler);
		ga->priv->iterate_handler = 0;
	}
	if (ga->priv->sb) {
		avahi_service_browser_free(ga->priv->sb);
		ga->priv->sb = NULL;
	}
	if (ga->priv->client) {
		avahi_client_free(ga->priv->client);
		ga->priv->client = NULL;
	}
	if (ga->priv->simple_poll) {
		avahi_simple_poll_free(ga->priv->simple_poll);
		ga->priv->simple_poll = NULL;
	}
	ga->started = FALSE;
}

void remmina_avahi_free(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
	if (ga == NULL)
		return;

	remmina_avahi_stop(ga);

	g_free(ga->priv);
	g_hash_table_destroy(ga->discovered_services);
	g_free(ga);
}

#else
/* Stub implementation when neither Avahi nor macOS Bonjour is available */

RemminaAvahi* remmina_avahi_new(void)
{
	TRACE_CALL(__func__);
	return NULL;
}

void remmina_avahi_start(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
}

void remmina_avahi_stop(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
}

void remmina_avahi_free(RemminaAvahi* ga)
{
	TRACE_CALL(__func__);
}

#endif

