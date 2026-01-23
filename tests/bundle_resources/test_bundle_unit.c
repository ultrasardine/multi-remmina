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
#include <glib.h>
#include <sys/stat.h>
#include "../../src/remmina_bundle_macos.h"

/* Test counter */
static gint tests_passed = 0;
static gint tests_failed = 0;

/* Test helper macros */
#define TEST_START(name) \
	g_print("Running test: %s\n", name);

#define TEST_ASSERT(condition, message) \
	do { \
		if (condition) { \
			tests_passed++; \
			g_print("  PASS: %s\n", message); \
		} else { \
			tests_failed++; \
			g_printerr("  FAIL: %s\n", message); \
		} \
	} while (0)

/* Test: Bundle resource path returns non-NULL */
static void test_bundle_resource_path_not_null(void)
{
	TEST_START("Bundle resource path returns non-NULL");
	
	gchar *path = remmina_get_bundle_resource_path();
	TEST_ASSERT(path != NULL, "Bundle resource path should not be NULL");
	
	if (path) {
		TEST_ASSERT(strlen(path) > 0, "Bundle resource path should not be empty");
		g_free(path);
	}
}

/* Test: Resource directory returns valid path */
static void test_resource_dir_valid(void)
{
	TEST_START("Resource directory returns valid path");
	
	gchar *dir = remmina_get_resource_dir();
	TEST_ASSERT(dir != NULL, "Resource directory should not be NULL");
	
	if (dir) {
		TEST_ASSERT(strlen(dir) > 0, "Resource directory should not be empty");
		g_free(dir);
	}
}

/* Test: Plugin directory returns valid path */
static void test_plugin_dir_valid(void)
{
	TEST_START("Plugin directory returns valid path");
	
	gchar *dir = remmina_get_plugin_dir();
	TEST_ASSERT(dir != NULL, "Plugin directory should not be NULL");
	
	if (dir) {
		TEST_ASSERT(strlen(dir) > 0, "Plugin directory should not be empty");
		TEST_ASSERT(strstr(dir, "plugins") != NULL, 
			    "Plugin directory should contain 'plugins'");
		g_free(dir);
	}
}

/* Test: UI directory returns valid path */
static void test_ui_dir_valid(void)
{
	TEST_START("UI directory returns valid path");
	
	gchar *dir = remmina_get_ui_dir();
	TEST_ASSERT(dir != NULL, "UI directory should not be NULL");
	
	if (dir) {
		TEST_ASSERT(strlen(dir) > 0, "UI directory should not be empty");
		TEST_ASSERT(strstr(dir, "ui") != NULL, 
			    "UI directory should contain 'ui'");
		g_free(dir);
	}
}

/* Test: Missing UI file handling */
static void test_missing_ui_file_handling(void)
{
	TEST_START("Missing UI file handling");
	
	gchar *ui_dir = remmina_get_ui_dir();
	if (!ui_dir) {
		TEST_ASSERT(FALSE, "Failed to get UI directory");
		return;
	}
	
	/* Try to construct path to non-existent file */
	gchar *missing_file = g_build_filename(ui_dir, "nonexistent_file.glade", NULL);
	TEST_ASSERT(missing_file != NULL, "Should be able to construct path to missing file");
	
	/* Verify the file doesn't exist */
	struct stat st;
	gboolean exists = (stat(missing_file, &st) == 0);
	TEST_ASSERT(!exists, "Non-existent file should not exist");
	
	g_free(missing_file);
	g_free(ui_dir);
}

/* Test: Missing theme file handling */
static void test_missing_theme_file_handling(void)
{
	TEST_START("Missing theme file handling");
	
	gchar *resource_dir = remmina_get_resource_dir();
	if (!resource_dir) {
		TEST_ASSERT(FALSE, "Failed to get resource directory");
		return;
	}
	
	/* Try to construct path to non-existent theme */
	gchar *missing_theme = g_build_filename(resource_dir, "theme", 
						"NonExistentTheme.colors", NULL);
	TEST_ASSERT(missing_theme != NULL, "Should be able to construct path to missing theme");
	
	/* Verify the file doesn't exist */
	struct stat st;
	gboolean exists = (stat(missing_theme, &st) == 0);
	TEST_ASSERT(!exists, "Non-existent theme should not exist");
	
	g_free(missing_theme);
	g_free(resource_dir);
}

/* Test: Missing icon handling */
static void test_missing_icon_handling(void)
{
	TEST_START("Missing icon handling");
	
	gchar *resource_dir = remmina_get_resource_dir();
	if (!resource_dir) {
		TEST_ASSERT(FALSE, "Failed to get resource directory");
		return;
	}
	
	/* Try to construct path to non-existent icon */
	gchar *missing_icon = g_build_filename(resource_dir, "icons", 
					       "nonexistent-icon.png", NULL);
	TEST_ASSERT(missing_icon != NULL, "Should be able to construct path to missing icon");
	
	/* Verify the file doesn't exist */
	struct stat st;
	gboolean exists = (stat(missing_icon, &st) == 0);
	TEST_ASSERT(!exists, "Non-existent icon should not exist");
	
	g_free(missing_icon);
	g_free(resource_dir);
}

/* Test: Path consistency across multiple calls */
static void test_path_consistency(void)
{
	TEST_START("Path consistency across multiple calls");
	
	gchar *dir1 = remmina_get_resource_dir();
	gchar *dir2 = remmina_get_resource_dir();
	
	TEST_ASSERT(dir1 != NULL && dir2 != NULL, "Both calls should return non-NULL");
	
	if (dir1 && dir2) {
		TEST_ASSERT(g_strcmp0(dir1, dir2) == 0, 
			    "Multiple calls should return same path");
	}
	
	g_free(dir1);
	g_free(dir2);
}

int main(int argc, char *argv[])
{
	g_print("=== Bundle Resource Unit Tests ===\n\n");
	
	/* Run all unit tests */
	test_bundle_resource_path_not_null();
	test_resource_dir_valid();
	test_plugin_dir_valid();
	test_ui_dir_valid();
	test_missing_ui_file_handling();
	test_missing_theme_file_handling();
	test_missing_icon_handling();
	test_path_consistency();
	
	/* Print summary */
	g_print("\n=== Test Summary ===\n");
	g_print("Passed: %d\n", tests_passed);
	g_print("Failed: %d\n", tests_failed);
	
	if (tests_failed == 0) {
		g_print("=== All unit tests PASSED ===\n");
		return 0;
	} else {
		g_print("=== Some unit tests FAILED ===\n");
		return 1;
	}
}

#else

#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Bundle resource unit tests are only available on macOS\n");
	return 0;
}

#endif /* __APPLE__ */
