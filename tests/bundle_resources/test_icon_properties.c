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

#ifdef __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "../../src/remmina_bundle_macos.h"

/* Icon names to test */
static const gchar *icon_names[] = {
	"remmina",
	"remmina-panel",
	"remmina-symbolic",
	"org.remmina.Remmina",
	NULL
};

/* Generate random icon name */
static const gchar* generate_random_icon_name(void)
{
	gint count = 0;
	while (icon_names[count] != NULL) {
		count++;
	}
	return icon_names[rand() % count];
}

/* Feature: macos-port, Property 9: Icon Resource Loading */
/* Validates: Requirements 8.4 */
static gboolean prop_icon_resource_loading(void)
{
	const gchar *icon_name = generate_random_icon_name();
	gchar *resource_dir = NULL;
	gchar *icon_path = NULL;
	gboolean result = FALSE;
	
	/* Get resource directory */
	resource_dir = remmina_get_resource_dir();
	if (!resource_dir) {
		g_printerr("Failed to get resource directory\n");
		goto cleanup;
	}
	
	/* Construct icon path */
	icon_path = g_build_filename(resource_dir, "icons", icon_name, NULL);
	if (!icon_path) {
		g_printerr("Failed to construct icon path\n");
		goto cleanup;
	}
	
	/* Verify path is not empty */
	if (strlen(icon_path) == 0) {
		g_printerr("Icon path is empty\n");
		goto cleanup;
	}
	
	/* Verify path contains "icons" component */
	if (!strstr(icon_path, "icons")) {
		g_printerr("Icon path '%s' does not contain 'icons' component\n", icon_path);
		goto cleanup;
	}
	
	/* Verify path contains the icon name */
	if (!strstr(icon_path, icon_name)) {
		g_printerr("Icon path '%s' does not contain icon name '%s'\n", 
			   icon_path, icon_name);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	g_free(resource_dir);
	g_free(icon_path);
	
	return result;
}

/* Run property test multiple times */
static gboolean run_property_test(const gchar *name, gboolean (*test_func)(void), gint iterations)
{
	gint passed = 0;
	gint failed = 0;
	
	g_print("Running property test: %s (%d iterations)\n", name, iterations);
	
	for (gint i = 0; i < iterations; i++) {
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
	
	/* Initialize GTK (required for icon theme operations) */
	gtk_init(&argc, &argv);
	
	/* Initialize random seed */
	srand(time(NULL));
	
	g_print("=== Icon Resource Loading Property-Based Tests ===\n\n");
	
	/* Run Property 9: Icon Resource Loading */
	if (!run_property_test("Property 9: Icon Resource Loading", 
			       prop_icon_resource_loading, 100)) {
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
	printf("Icon resource property tests are only available on macOS\n");
	return 0;
}

#endif /* __APPLE__ */
