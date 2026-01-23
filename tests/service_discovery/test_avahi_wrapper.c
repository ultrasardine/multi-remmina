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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "../../src/remmina_avahi.h"

/* Test counter */
static gint tests_passed = 0;
static gint tests_failed = 0;

/* Helper to run a test */
static void run_test(const gchar *name, gboolean (*test_func)(void))
{
	g_print("Running: %s... ", name);
	if (test_func()) {
		g_print("PASSED\n");
		tests_passed++;
	} else {
		g_print("FAILED\n");
		tests_failed++;
	}
}

/* Test: Avahi wrapper can be created */
static gboolean test_avahi_new(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	/* Verify structure is initialized */
	if (!ga->discovered_services) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	if (ga->started) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	remmina_avahi_free(ga);
	return TRUE;
}

/* Test: Avahi wrapper can start service discovery */
static gboolean test_avahi_start(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	remmina_avahi_start(ga);
	
	/* Verify started flag is set */
	if (!ga->started) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	remmina_avahi_free(ga);
	return TRUE;
}

/* Test: Avahi wrapper can stop service discovery */
static gboolean test_avahi_stop(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	remmina_avahi_start(ga);
	remmina_avahi_stop(ga);
	
	/* Verify started flag is cleared */
	if (ga->started) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	/* Verify hash table is empty */
	if (g_hash_table_size(ga->discovered_services) != 0) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	remmina_avahi_free(ga);
	return TRUE;
}

/* Test: Avahi wrapper can be freed safely */
static gboolean test_avahi_free(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	remmina_avahi_start(ga);
	remmina_avahi_free(ga);
	
	/* If we get here without crashing, test passes */
	return TRUE;
}

/* Test: Avahi wrapper can be freed without starting */
static gboolean test_avahi_free_without_start(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	remmina_avahi_free(ga);
	
	/* If we get here without crashing, test passes */
	return TRUE;
}

/* Test: Avahi wrapper handles NULL gracefully */
static gboolean test_avahi_free_null(void)
{
	/* Should not crash */
	remmina_avahi_free(NULL);
	return TRUE;
}

/* Test: Avahi wrapper can start multiple times */
static gboolean test_avahi_start_multiple_times(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	remmina_avahi_start(ga);
	remmina_avahi_start(ga); /* Should be safe */
	
	/* Verify started flag is still set */
	if (!ga->started) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	remmina_avahi_free(ga);
	return TRUE;
}

/* Test: Avahi wrapper can stop multiple times */
static gboolean test_avahi_stop_multiple_times(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	remmina_avahi_start(ga);
	remmina_avahi_stop(ga);
	remmina_avahi_stop(ga); /* Should be safe */
	
	/* Verify started flag is still cleared */
	if (ga->started) {
		remmina_avahi_free(ga);
		return FALSE;
	}
	
	remmina_avahi_free(ga);
	return TRUE;
}

/* Test: Avahi wrapper integrates with Bonjour */
static gboolean test_avahi_bonjour_integration(void)
{
	RemminaAvahi *ga = remmina_avahi_new();
	if (!ga) {
		return FALSE;
	}
	
	/* Start service discovery */
	remmina_avahi_start(ga);
	
	/* Wait a bit for services to be discovered */
	/* Note: This is a simple test, not a comprehensive one */
	g_usleep(100000); /* 100ms */
	
	/* Process GLib main loop events */
	GMainContext *context = g_main_context_default();
	while (g_main_context_iteration(context, FALSE)) {
		/* Process events */
	}
	
	/* We don't check for discovered services because there might not be any */
	/* The test passes if we don't crash */
	
	remmina_avahi_free(ga);
	return TRUE;
}

int main(int argc, char *argv[])
{
	g_print("=== Avahi Wrapper Integration Tests ===\n\n");
	
	/* Basic functionality tests */
	run_test("Avahi wrapper can be created", test_avahi_new);
	run_test("Avahi wrapper can start service discovery", test_avahi_start);
	run_test("Avahi wrapper can stop service discovery", test_avahi_stop);
	run_test("Avahi wrapper can be freed safely", test_avahi_free);
	run_test("Avahi wrapper can be freed without starting", test_avahi_free_without_start);
	run_test("Avahi wrapper handles NULL gracefully", test_avahi_free_null);
	
	/* Edge case tests */
	run_test("Avahi wrapper can start multiple times", test_avahi_start_multiple_times);
	run_test("Avahi wrapper can stop multiple times", test_avahi_stop_multiple_times);
	
	/* Integration test */
	run_test("Avahi wrapper integrates with Bonjour", test_avahi_bonjour_integration);
	
	/* Print summary */
	g_print("\n=== Test Summary ===\n");
	g_print("Passed: %d\n", tests_passed);
	g_print("Failed: %d\n", tests_failed);
	
	if (tests_failed == 0) {
		g_print("\n=== All Avahi wrapper tests PASSED ===\n");
		return 0;
	} else {
		g_print("\n=== Some Avahi wrapper tests FAILED ===\n");
		return 1;
	}
}

#else

#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Avahi wrapper tests are only available on macOS\n");
	return 0;
}

#endif /* __APPLE__ */
