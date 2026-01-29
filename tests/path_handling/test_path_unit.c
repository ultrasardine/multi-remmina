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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "../../src/remmina_paths_windows.h"

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

/* Test: UNC paths (\\server\share) */
/* Validates: Requirements 13.5 */
static void test_unc_paths(void)
{
	TEST_START("UNC paths (\\\\server\\share)");
	
	gchar *normalized = NULL;
	
	/* Test basic UNC path */
	normalized = remmina_paths_normalize("\\\\server\\share\\folder");
	TEST_ASSERT(normalized != NULL, "UNC path normalized successfully");
	TEST_ASSERT(normalized && g_str_has_prefix(normalized, "\\\\"), 
		    "UNC prefix preserved");
	g_free(normalized);
	
	/* Test UNC path with forward slashes */
	normalized = remmina_paths_normalize("//server/share/folder");
	TEST_ASSERT(normalized != NULL, "UNC path with forward slashes normalized");
	TEST_ASSERT(normalized && g_str_has_prefix(normalized, "\\\\"), 
		    "Forward slash UNC converted to backslash");
	g_free(normalized);
	
	/* Test UNC path with mixed slashes */
	normalized = remmina_paths_normalize("\\\\server/share\\folder/file");
	TEST_ASSERT(normalized != NULL, "Mixed UNC path normalized");
	TEST_ASSERT(normalized && !strstr(normalized, "/"), 
		    "No forward slashes in normalized UNC path");
	g_free(normalized);
}

/* Test: Paths with spaces */
/* Validates: Requirements 13.5 */
static void test_paths_with_spaces(void)
{
	TEST_START("Paths with spaces");
	
	gchar *normalized = NULL;
	
	/* Test path with spaces */
	normalized = remmina_paths_normalize("C:/Program Files/Multi Remmina/app.exe");
	TEST_ASSERT(normalized != NULL, "Path with spaces normalized");
	TEST_ASSERT(normalized && strstr(normalized, "Program Files"), 
		    "Spaces preserved in path");
	TEST_ASSERT(normalized && strstr(normalized, "Multi Remmina"), 
		    "Multiple spaces preserved");
	g_free(normalized);
	
	/* Test path with leading/trailing spaces in components */
	normalized = remmina_paths_normalize("C:\\ folder \\file.txt");
	TEST_ASSERT(normalized != NULL, "Path with space-padded components normalized");
	g_free(normalized);
}

/* Test: Non-existent environment variables */
/* Validates: Requirements 13.5 */
static void test_nonexistent_env_vars(void)
{
	TEST_START("Non-existent environment variables");
	
	gchar *expanded = NULL;
	
	/* Test non-existent environment variable */
	expanded = remmina_paths_expand_env("%NONEXISTENT_VAR_12345%\\test");
	TEST_ASSERT(expanded != NULL, "Non-existent env var returns result");
	/* Windows keeps unexpanded variables as-is */
	TEST_ASSERT(expanded && strstr(expanded, "NONEXISTENT_VAR_12345"), 
		    "Non-existent env var kept in path");
	g_free(expanded);
	
	/* Test mixed existing and non-existing */
	expanded = remmina_paths_expand_env("%APPDATA%\\%NONEXISTENT_VAR%\\test");
	TEST_ASSERT(expanded != NULL, "Mixed env vars returns result");
	/* APPDATA should be expanded, NONEXISTENT should remain */
	TEST_ASSERT(expanded && !strstr(expanded, "%APPDATA%"), 
		    "Existing env var expanded");
	g_free(expanded);
}

/* Test: NULL parameter handling */
static void test_null_parameters(void)
{
	TEST_START("NULL parameter handling");
	
	gchar *result;
	
	/* Test normalize with NULL */
	result = remmina_paths_normalize(NULL);
	TEST_ASSERT(result == NULL, "Normalize NULL returns NULL");
	
	/* Test expand_env with NULL */
	result = remmina_paths_expand_env(NULL);
	TEST_ASSERT(result == NULL, "Expand env NULL returns NULL");
}

/* Test: Empty path handling */
static void test_empty_paths(void)
{
	TEST_START("Empty path handling");
	
	gchar *result;
	
	/* Test normalize with empty string */
	result = remmina_paths_normalize("");
	TEST_ASSERT(result != NULL, "Normalize empty string returns non-NULL");
	TEST_ASSERT(result && strlen(result) == 0, "Normalize empty string returns empty");
	g_free(result);
	
	/* Test expand_env with empty string */
	result = remmina_paths_expand_env("");
	TEST_ASSERT(result != NULL, "Expand env empty string returns non-NULL");
	TEST_ASSERT(result && strlen(result) == 0, "Expand env empty string returns empty");
	g_free(result);
}

/* Test: Path directory functions */
static void test_path_directories(void)
{
	TEST_START("Path directory functions");
	
	gchar *path;
	
	/* Test install dir */
	path = remmina_paths_get_install_dir();
	TEST_ASSERT(path != NULL, "Install dir returns non-NULL");
	TEST_ASSERT(path && strlen(path) > 0, "Install dir is not empty");
	g_free(path);
	
	/* Test config dir */
	path = remmina_paths_get_config_dir();
	TEST_ASSERT(path != NULL, "Config dir returns non-NULL");
	TEST_ASSERT(path && strstr(path, "multi-remmina"), 
		    "Config dir contains multi-remmina");
	g_free(path);
	
	/* Test data dir */
	path = remmina_paths_get_data_dir();
	TEST_ASSERT(path != NULL, "Data dir returns non-NULL");
	TEST_ASSERT(path && strstr(path, "multi-remmina"), 
		    "Data dir contains multi-remmina");
	g_free(path);
	
	/* Test plugin dir */
	path = remmina_paths_get_plugin_dir();
	TEST_ASSERT(path != NULL, "Plugin dir returns non-NULL");
	TEST_ASSERT(path && strstr(path, "plugins"), 
		    "Plugin dir contains plugins");
	g_free(path);
	
	/* Test UI dir */
	path = remmina_paths_get_ui_dir();
	TEST_ASSERT(path != NULL, "UI dir returns non-NULL");
	TEST_ASSERT(path && strstr(path, "ui"), 
		    "UI dir contains ui");
	g_free(path);
	
	/* Test theme dir */
	path = remmina_paths_get_theme_dir();
	TEST_ASSERT(path != NULL, "Theme dir returns non-NULL");
	TEST_ASSERT(path && strstr(path, "theme"), 
		    "Theme dir contains theme");
	g_free(path);
}

/* Test: Duplicate separator handling */
static void test_duplicate_separators(void)
{
	TEST_START("Duplicate separator handling");
	
	gchar *normalized;
	
	/* Test multiple consecutive backslashes */
	normalized = remmina_paths_normalize("C:\\\\folder\\\\\\file.txt");
	TEST_ASSERT(normalized != NULL, "Multiple backslashes normalized");
	/* Should not have triple backslashes */
	TEST_ASSERT(normalized && !strstr(normalized, "\\\\\\"), 
		    "Triple backslashes removed");
	g_free(normalized);
	
	/* Test multiple consecutive forward slashes */
	normalized = remmina_paths_normalize("C:///folder///file.txt");
	TEST_ASSERT(normalized != NULL, "Multiple forward slashes normalized");
	TEST_ASSERT(normalized && !strstr(normalized, "\\\\\\"), 
		    "Multiple slashes collapsed");
	g_free(normalized);
}

/* Test: Drive letter handling */
static void test_drive_letters(void)
{
	TEST_START("Drive letter handling");
	
	gchar *normalized;
	
	/* Test various drive letters */
	normalized = remmina_paths_normalize("D:/folder/file.txt");
	TEST_ASSERT(normalized != NULL, "D: drive path normalized");
	TEST_ASSERT(normalized && normalized[0] == 'D', "Drive letter preserved");
	g_free(normalized);
	
	/* Test lowercase drive letter */
	normalized = remmina_paths_normalize("c:/folder/file.txt");
	TEST_ASSERT(normalized != NULL, "Lowercase drive letter normalized");
	TEST_ASSERT(normalized && normalized[0] == 'c', "Lowercase drive letter preserved");
	g_free(normalized);
}

int main(int argc, char *argv[])
{
	/* Initialize path handling */
	if (!remmina_paths_windows_init()) {
		g_printerr("Failed to initialize Windows path handling\n");
		return 1;
	}
	
	g_print("=== Windows Path Handling Unit Tests (Edge Cases) ===\n\n");
	
	/* Run all tests */
	test_unc_paths();
	g_print("\n");
	
	test_paths_with_spaces();
	g_print("\n");
	
	test_nonexistent_env_vars();
	g_print("\n");
	
	test_null_parameters();
	g_print("\n");
	
	test_empty_paths();
	g_print("\n");
	
	test_path_directories();
	g_print("\n");
	
	test_duplicate_separators();
	g_print("\n");
	
	test_drive_letters();
	g_print("\n");
	
	/* Print summary */
	g_print("=== Test Summary ===\n");
	g_print("Passed: %d\n", tests_passed);
	g_print("Failed: %d\n", tests_failed);
	
	if (tests_failed == 0) {
		g_print("\n=== All unit tests PASSED ===\n");
		return 0;
	} else {
		g_print("\n=== Some unit tests FAILED ===\n");
		return 1;
	}
}

#else

#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Path handling unit tests are only available on Windows\n");
	return 0;
}

#endif /* _WIN32 */
