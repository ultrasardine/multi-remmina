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

#if defined(__APPLE__) || defined(_WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>

#ifdef __APPLE__
#include "../../src/remmina_bonjour_macos.h"
#define PLATFORM_NAME "macOS"
#define service_discovery_init remmina_service_discovery_init
#define service_discovery_browse remmina_service_discovery_browse
#define service_discovery_stop remmina_service_discovery_stop
typedef RemminaServiceCallback ServiceCallback;
#elif defined(_WIN32)
#include "../../src/remmina_dnssd_windows.h"
#define PLATFORM_NAME "Windows"
#define service_discovery_init remmina_dnssd_windows_init
#define service_discovery_browse remmina_dnssd_windows_browse
#define service_discovery_stop remmina_dnssd_windows_stop
typedef RemminaDnssdCallback ServiceCallback;
#endif

/* Test data structure */
typedef struct {
	GSList *discovered_services;
	GMainLoop *loop;
	guint timeout_id;
	gint expected_count;
} ServiceDiscoveryTestData;

/* Structure to hold discovered service info */
typedef struct {
	gchar *service_name;
	gchar *hostname;
	guint16 port;
} DiscoveredService;

/* Free discovered service */
static void free_discovered_service(gpointer data)
{
	DiscoveredService *service = (DiscoveredService *)data;
	if (service) {
		g_free(service->service_name);
		g_free(service->hostname);
		g_free(service);
	}
}

/* Callback for discovered services */
static void service_discovered_callback(const gchar *service_name,
					const gchar *hostname,
					guint16 port,
					gpointer user_data)
{
	ServiceDiscoveryTestData *test_data = (ServiceDiscoveryTestData *)user_data;
	
	/* Create discovered service record */
	DiscoveredService *service = g_malloc0(sizeof(DiscoveredService));
	service->service_name = g_strdup(service_name);
	service->hostname = g_strdup(hostname);
	service->port = port;
	
	/* Add to list */
	test_data->discovered_services = g_slist_append(test_data->discovered_services, service);
	
	g_print("  Discovered: %s at %s:%d\n", service_name, hostname, port);
}

/* Timeout callback to stop the main loop */
static gboolean timeout_callback(gpointer user_data)
{
	ServiceDiscoveryTestData *test_data = (ServiceDiscoveryTestData *)user_data;
	g_main_loop_quit(test_data->loop);
	return FALSE;
}

/* Feature: windows-port/macos-port, Property 8: Service Discovery Display */
/* Validates: Requirements 7.2, 7.3 (Windows), 11.2, 11.3 (macOS) */
static gboolean prop_service_discovery_display(void)
{
	ServiceDiscoveryTestData test_data = {0};
	gboolean result = FALSE;
	
	/* Initialize test data */
	test_data.discovered_services = NULL;
	test_data.loop = g_main_loop_new(NULL, FALSE);
	test_data.expected_count = 0; /* We don't know how many services exist */
	
#ifdef _WIN32
	/* Check if DNS-SD is available on this Windows version */
	if (!remmina_dnssd_windows_is_available()) {
		g_print("  DNS-SD not available on this Windows version, skipping\n");
		result = TRUE; /* Pass the test - DNS-SD unavailable is acceptable */
		goto cleanup;
	}
#endif
	
	/* Initialize service discovery */
	if (!service_discovery_init()) {
		g_printerr("Failed to initialize service discovery\n");
		goto cleanup;
	}
	
	/* Start browsing for VNC services */
	if (!service_discovery_browse("_rfb._tcp", 
				       service_discovered_callback, 
				       &test_data)) {
		g_printerr("Failed to start service browsing\n");
		goto cleanup;
	}
	
	/* Set timeout to stop browsing after 3 seconds */
	test_data.timeout_id = g_timeout_add_seconds(3, timeout_callback, &test_data);
	
	/* Run main loop to process discoveries */
	g_main_loop_run(test_data.loop);
	
	/* Check if any services were discovered */
	gint count = g_slist_length(test_data.discovered_services);
	g_print("  Discovered %d service(s)\n", count);
	
	/* Verify each discovered service has valid data */
	GSList *iter;
	for (iter = test_data.discovered_services; iter != NULL; iter = iter->next) {
		DiscoveredService *service = (DiscoveredService *)iter->data;
		
		if (!service->service_name || strlen(service->service_name) == 0) {
			g_printerr("Service has empty name\n");
			goto cleanup;
		}
		
		if (!service->hostname || strlen(service->hostname) == 0) {
			g_printerr("Service has empty hostname\n");
			goto cleanup;
		}
		
		if (service->port == 0) {
			g_printerr("Service has invalid port\n");
			goto cleanup;
		}
	}
	
	/* Test passes if we can browse (even if no services found) */
	/* and all discovered services have valid data */
	result = TRUE;
	
cleanup:
	/* Stop service discovery */
	service_discovery_stop();
	
	/* Clean up */
	if (test_data.timeout_id > 0) {
		g_source_remove(test_data.timeout_id);
	}
	if (test_data.loop) {
		g_main_loop_unref(test_data.loop);
	}
	g_slist_free_full(test_data.discovered_services, free_discovered_service);
	
	return result;
}

/* Run property test multiple times */
static gboolean run_property_test(const gchar *name, gboolean (*test_func)(void), gint iterations)
{
	gint passed = 0;
	gint failed = 0;
	
	g_print("Running property test: %s (%d iterations)\n", name, iterations);
	
	for (gint i = 0; i < iterations; i++) {
		g_print("Iteration %d:\n", i + 1);
		if (test_func()) {
			passed++;
		} else {
			failed++;
			g_printerr("  Iteration %d FAILED\n", i + 1);
		}
	}
	
	g_print("  Results: %d passed, %d failed\n", passed, failed);
	
	return (failed == 0);
}

int main(int argc, char *argv[])
{
	gint exit_code = 0;
	
	/* Initialize random seed */
	srand(time(NULL));
	
	g_print("=== Service Discovery Property-Based Tests (%s) ===\n\n", PLATFORM_NAME);
	
	/* Run Property 8: Service Discovery Display */
	/* Note: Running 100 iterations would take too long (3 seconds each) */
	/* So we run fewer iterations for this test */
	if (!run_property_test("Property 8: Service Discovery Display", 
			       prop_service_discovery_display, 5)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (exit_code == 0) {
		g_print("=== All property tests PASSED ===\n");
	} else {
		g_print("=== Some property tests FAILED ===\n");
	}
	
	return exit_code;
}

#else

#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Service discovery property tests are only available on macOS and Windows\n");
	return 0;
}

#endif /* __APPLE__ || _WIN32 */
