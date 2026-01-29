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
#include <time.h>
#include <glib.h>
#include "../../src/remmina_paths_windows.h"

/* Simple property-based testing framework */
typedef struct {
	gchar *path_forward;     /* Path with forward slashes */
	gchar *path_backward;    /* Path with backslashes */
	gchar *path_mixed;       /* Path with mixed separators */
} PathTestData;

/* Generate random path component */
static gchar* generate_random_component(gint min_len, gint max_len)
{
	static const gchar charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
	gint len = min_len + (rand() % (max_len - min_len + 1));
	gchar *str = g_malloc0(len + 1);
	
	for (gint i = 0; i < len; i++) {
		str[i] = charset[rand() % (sizeof(charset) - 1)];
	}
	str[len] = '\0';
	
	return str;
}

/* Generate test path data with different separator styles */
static PathTestData* generate_path_test_data(void)
{
	PathTestData *data = g_malloc0(sizeof(PathTestData));
	gint num_components = 2 + (rand() % 4);  /* 2-5 components */
	GString *forward = g_string_new("C:");
	GString *backward = g_string_new("C:");
	GString *mixed = g_string_new("C:");
	
	for (gint i = 0; i < num_components; i++) {
		gchar *component = generate_random_component(3, 12);
		
		g_string_append_c(forward, '/');
		g_string_append(forward, component);
		
		g_string_append_c(backward, '\\');
		g_string_append(backward, component);
		
		/* Alternate separators for mixed */
		g_string_append_c(mixed, (i % 2 == 0) ? '/' : '\\');
		g_string_append(mixed, component);
		
		g_free(component);
	}
	
	data->path_forward = g_string_free(forward, FALSE);
	data->path_backward = g_string_free(backward, FALSE);
	data->path_mixed = g_string_free(mixed, FALSE);
	
	return data;
}

/* Free test data */
static void free_path_test_data(PathTestData *data)
{
	if (data) {
		g_free(data->path_forward);
		g_free(data->path_backward);
		g_free(data->path_mixed);
		g_free(data);
	}
}


/* Feature: windows-port, Property 9: Path Separator Normalization */
/* Validates: Requirements 13.1 */
static gboolean prop_path_separator_normalization(void)
{
	PathTestData *data = generate_path_test_data();
	gchar *norm_forward = NULL;
	gchar *norm_backward = NULL;
	gchar *norm_mixed = NULL;
	gboolean result = FALSE;
	
	/* Normalize all three path variants */
	norm_forward = remmina_paths_normalize(data->path_forward);
	norm_backward = remmina_paths_normalize(data->path_backward);
	norm_mixed = remmina_paths_normalize(data->path_mixed);
	
	if (!norm_forward || !norm_backward || !norm_mixed) {
		g_printerr("Failed to normalize paths\n");
		goto cleanup;
	}
	
	/* All normalized paths should be identical */
	if (g_strcmp0(norm_forward, norm_backward) != 0) {
		g_printerr("Forward and backward normalized paths differ:\n");
		g_printerr("  Forward:  '%s'\n", norm_forward);
		g_printerr("  Backward: '%s'\n", norm_backward);
		goto cleanup;
	}
	
	if (g_strcmp0(norm_forward, norm_mixed) != 0) {
		g_printerr("Forward and mixed normalized paths differ:\n");
		g_printerr("  Forward: '%s'\n", norm_forward);
		g_printerr("  Mixed:   '%s'\n", norm_mixed);
		goto cleanup;
	}
	
	/* Normalized path should only contain backslashes (except drive letter) */
	for (gchar *p = norm_forward + 2; *p; p++) {  /* Skip "C:" */
		if (*p == '/') {
			g_printerr("Normalized path still contains forward slash: '%s'\n", 
				   norm_forward);
			goto cleanup;
		}
	}
	
	result = TRUE;
	
cleanup:
	g_free(norm_forward);
	g_free(norm_backward);
	g_free(norm_mixed);
	free_path_test_data(data);
	
	return result;
}

/* Feature: windows-port, Property 10: Environment Variable Expansion */
/* Validates: Requirements 13.4 */
static gboolean prop_env_variable_expansion(void)
{
	gboolean result = FALSE;
	gchar *expanded = NULL;
	gchar *appdata_path = NULL;
	gchar *localappdata_path = NULL;
	
	/* Test %APPDATA% expansion */
	expanded = remmina_paths_expand_env("%APPDATA%\\test");
	if (!expanded) {
		g_printerr("Failed to expand %%APPDATA%%\n");
		goto cleanup;
	}
	
	/* Verify it doesn't contain the literal %APPDATA% anymore */
	if (strstr(expanded, "%APPDATA%") != NULL) {
		g_printerr("%%APPDATA%% was not expanded: '%s'\n", expanded);
		g_free(expanded);
		goto cleanup;
	}
	
	/* Verify it ends with \test */
	if (!g_str_has_suffix(expanded, "\\test")) {
		g_printerr("Expanded path doesn't end with \\test: '%s'\n", expanded);
		g_free(expanded);
		goto cleanup;
	}
	
	g_free(expanded);
	
	/* Test %LOCALAPPDATA% expansion */
	expanded = remmina_paths_expand_env("%LOCALAPPDATA%\\test");
	if (!expanded) {
		g_printerr("Failed to expand %%LOCALAPPDATA%%\n");
		goto cleanup;
	}
	
	/* Verify it doesn't contain the literal %LOCALAPPDATA% anymore */
	if (strstr(expanded, "%LOCALAPPDATA%") != NULL) {
		g_printerr("%%LOCALAPPDATA%% was not expanded: '%s'\n", expanded);
		g_free(expanded);
		goto cleanup;
	}
	
	g_free(expanded);
	
	/* Test %USERPROFILE% expansion */
	expanded = remmina_paths_expand_env("%USERPROFILE%\\Documents");
	if (!expanded) {
		g_printerr("Failed to expand %%USERPROFILE%%\n");
		goto cleanup;
	}
	
	/* Verify it doesn't contain the literal %USERPROFILE% anymore */
	if (strstr(expanded, "%USERPROFILE%") != NULL) {
		g_printerr("%%USERPROFILE%% was not expanded: '%s'\n", expanded);
		g_free(expanded);
		goto cleanup;
	}
	
	g_free(expanded);
	expanded = NULL;
	
	result = TRUE;
	
cleanup:
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
	
	/* Initialize random seed */
	srand((unsigned int)time(NULL));
	
	/* Initialize path handling */
	if (!remmina_paths_windows_init()) {
		g_printerr("Failed to initialize Windows path handling\n");
		return 1;
	}
	
	g_print("=== Windows Path Handling Property-Based Tests ===\n\n");
	
	/* Run Property 9: Path Separator Normalization */
	if (!run_property_test("Property 9: Path Separator Normalization", 
			       prop_path_separator_normalization, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Run Property 10: Environment Variable Expansion */
	if (!run_property_test("Property 10: Environment Variable Expansion", 
			       prop_env_variable_expansion, 100)) {
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
	printf("Path handling property tests are only available on Windows\n");
	return 0;
}

#endif /* _WIN32 */
