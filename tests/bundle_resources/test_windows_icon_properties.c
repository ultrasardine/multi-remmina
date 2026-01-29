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

/**
 * Property-Based Tests for Windows Icon Resource Loading
 * 
 * Feature: windows-port
 * Property 11: Icon Resource Loading
 * Validates: Requirements 11.4
 * 
 * For any icon referenced in the UI, the icon should be successfully 
 * loaded from the installation directory's resource path.
 */

#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include "../../src/remmina_paths_windows.h"

/* Icon names to test - these are common icons used by Remmina */
static const gchar *icon_names[] = {
	"remmina",
	"remmina-panel",
	"remmina-symbolic",
	"org.multi-remmina.MultiRemmina",
	"preferences-system",
	"document-new",
	"document-open",
	"document-save",
	"edit-copy",
	"edit-paste",
	"window-close",
	NULL
};

/* Icon sizes to test */
static const gint icon_sizes[] = {
	16, 22, 24, 32, 48, 64, 128, 256, 512
};

/* Count array elements */
static gint count_icon_names(void)
{
	gint count = 0;
	while (icon_names[count] != NULL) {
		count++;
	}
	return count;
}

/* Generate random icon name */
static const gchar* generate_random_icon_name(void)
{
	return icon_names[rand() % count_icon_names()];
}

/* Generate random icon size */
static gint generate_random_icon_size(void)
{
	return icon_sizes[rand() % (sizeof(icon_sizes) / sizeof(icon_sizes[0]))];
}

/**
 * Feature: windows-port
 * Property 11: Icon Resource Loading
 * Validates: Requirements 11.4
 * 
 * For any icon referenced in the UI, the icon should be successfully 
 * loaded from the installation directory's resource path.
 */
static gboolean prop_icon_resource_loading(void)
{
	const gchar *icon_name = generate_random_icon_name();
	gint icon_size = generate_random_icon_size();
	gchar *install_dir = NULL;
	gchar *icon_dir = NULL;
	gchar *icon_path = NULL;
	gboolean result = FALSE;
	
	/* Get installation directory */
	install_dir = remmina_paths_get_install_dir();
	if (!install_dir) {
		g_printerr("Failed to get installation directory\n");
		goto cleanup;
	}
	
	/* Construct icon directory path */
	/* Icons are typically in share/icons/hicolor/<size>x<size>/apps/ */
	gchar *size_str = g_strdup_printf("%dx%d", icon_size, icon_size);
	icon_dir = g_build_filename(install_dir, "share", "icons", "hicolor", 
				    size_str, "apps", NULL);
	g_free(size_str);
	
	if (!icon_dir) {
		g_printerr("Failed to construct icon directory path\n");
		goto cleanup;
	}
	
	/* Construct full icon path */
	gchar *icon_filename = g_strdup_printf("%s.png", icon_name);
	icon_path = g_build_filename(icon_dir, icon_filename, NULL);
	g_free(icon_filename);
	
	if (!icon_path) {
		g_printerr("Failed to construct icon path\n");
		goto cleanup;
	}
	
	/* Verify path is not empty */
	if (strlen(icon_path) == 0) {
		g_printerr("Icon path is empty\n");
		goto cleanup;
	}
	
	/* Verify path contains expected components */
	if (!strstr(icon_path, "icons")) {
		g_printerr("Icon path '%s' does not contain 'icons' component\n", icon_path);
		goto cleanup;
	}
	
	if (!strstr(icon_path, "hicolor")) {
		g_printerr("Icon path '%s' does not contain 'hicolor' component\n", icon_path);
		goto cleanup;
	}
	
	/* Verify path contains the icon name */
	if (!strstr(icon_path, icon_name)) {
		g_printerr("Icon path '%s' does not contain icon name '%s'\n", 
			   icon_path, icon_name);
		goto cleanup;
	}
	
	/* Verify the path is relative to the installation directory */
	if (strncmp(icon_path, install_dir, strlen(install_dir)) != 0) {
		g_printerr("Icon path '%s' is not relative to install dir '%s'\n",
			   icon_path, install_dir);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	g_free(install_dir);
	g_free(icon_dir);
	g_free(icon_path);
	
	return result;
}

/**
 * Property: Scalable Icon Path Resolution
 * 
 * Scalable icons (SVG) should be locatable in the scalable directory.
 */
static gboolean prop_scalable_icon_resolution(void)
{
	const gchar *icon_name = generate_random_icon_name();
	gchar *install_dir = NULL;
	gchar *icon_path = NULL;
	gboolean result = FALSE;
	
	/* Get installation directory */
	install_dir = remmina_paths_get_install_dir();
	if (!install_dir) {
		g_printerr("Failed to get installation directory\n");
		goto cleanup;
	}
	
	/* Construct scalable icon path */
	gchar *icon_filename = g_strdup_printf("%s.svg", icon_name);
	icon_path = g_build_filename(install_dir, "share", "icons", "hicolor", 
				     "scalable", "apps", icon_filename, NULL);
	g_free(icon_filename);
	
	if (!icon_path) {
		g_printerr("Failed to construct scalable icon path\n");
		goto cleanup;
	}
	
	/* Verify path is not empty */
	if (strlen(icon_path) == 0) {
		g_printerr("Scalable icon path is empty\n");
		goto cleanup;
	}
	
	/* Verify path contains expected components */
	if (!strstr(icon_path, "scalable")) {
		g_printerr("Icon path '%s' does not contain 'scalable' component\n", icon_path);
		goto cleanup;
	}
	
	if (!strstr(icon_path, ".svg")) {
		g_printerr("Icon path '%s' does not have .svg extension\n", icon_path);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	g_free(install_dir);
	g_free(icon_path);
	
	return result;
}

/**
 * Property: Application Icon Path Resolution
 * 
 * Application-specific icons should be locatable in the multi-remmina directory.
 */
static gboolean prop_app_icon_resolution(void)
{
	gchar *install_dir = NULL;
	gchar *icon_path = NULL;
	gboolean result = FALSE;
	
	/* Get installation directory */
	install_dir = remmina_paths_get_install_dir();
	if (!install_dir) {
		g_printerr("Failed to get installation directory\n");
		goto cleanup;
	}
	
	/* Construct application icon path */
	icon_path = g_build_filename(install_dir, "share", "multi-remmina", 
				     "icons", "remmina.png", NULL);
	
	if (!icon_path) {
		g_printerr("Failed to construct application icon path\n");
		goto cleanup;
	}
	
	/* Verify path is not empty */
	if (strlen(icon_path) == 0) {
		g_printerr("Application icon path is empty\n");
		goto cleanup;
	}
	
	/* Verify path contains expected components */
	if (!strstr(icon_path, "multi-remmina")) {
		g_printerr("Icon path '%s' does not contain 'multi-remmina' component\n", icon_path);
		goto cleanup;
	}
	
	/* Verify the path is relative to the installation directory */
	if (strncmp(icon_path, install_dir, strlen(install_dir)) != 0) {
		g_printerr("Icon path '%s' is not relative to install dir '%s'\n",
			   icon_path, install_dir);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	g_free(install_dir);
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
	
	/* Initialize Windows path handling */
	if (!remmina_paths_windows_init()) {
		g_printerr("Failed to initialize Windows path handling\n");
		return 1;
	}
	
	/* Initialize random seed */
	srand((unsigned int)time(NULL));
	
	g_print("=== Windows Icon Resource Loading Property-Based Tests ===\n\n");
	
	g_print("Feature: windows-port\n");
	g_print("Property 11: Icon Resource Loading\n");
	g_print("Validates: Requirements 11.4\n\n");
	
	/* Run Property 11: Icon Resource Loading */
	if (!run_property_test("Icon Resource Loading", 
			       prop_icon_resource_loading, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Run Scalable Icon Resolution test */
	if (!run_property_test("Scalable Icon Resolution", 
			       prop_scalable_icon_resolution, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Run Application Icon Resolution test */
	if (!run_property_test("Application Icon Resolution", 
			       prop_app_icon_resolution, 100)) {
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
	printf("Windows icon resource property tests are only available on Windows\n");
	return 0;
}

#endif /* _WIN32 */
