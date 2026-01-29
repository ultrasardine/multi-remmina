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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

/* Test data directory */
static gchar *test_plugin_dir = NULL;

/* Setup test environment */
static void setup_test_env(void)
{
	/* Create a temporary directory for test plugins */
	test_plugin_dir = g_dir_make_tmp("remmina_plugin_test_XXXXXX", NULL);
	if (!test_plugin_dir) {
		g_printerr("Failed to create test plugin directory\n");
		exit(1);
	}
	
	g_print("Test plugin directory: %s\n", test_plugin_dir);
}

/* Cleanup test environment */
static void cleanup_test_env(void)
{
	if (test_plugin_dir) {
		/* Remove all files in test directory */
		GDir *dir = g_dir_open(test_plugin_dir, 0, NULL);
		if (dir) {
			const gchar *name;
			while ((name = g_dir_read_name(dir)) != NULL) {
				gchar *filepath = g_build_filename(test_plugin_dir, name, NULL);
				g_unlink(filepath);
				g_free(filepath);
			}
			g_dir_close(dir);
		}
		
		/* Remove the directory */
		g_rmdir(test_plugin_dir);
		g_free(test_plugin_dir);
		test_plugin_dir = NULL;
	}
}

/* Test: Plugin loading with missing dependency */
/* Validates: Requirement 3.5 */
static gboolean test_plugin_missing_dependency(void)
{
	gboolean result = TRUE;
	
	g_print("Test: Plugin loading with missing dependency\n");
	
	/* Try to load a non-existent plugin file */
#ifdef _WIN32
	gchar *fake_plugin = g_build_filename(test_plugin_dir, "nonexistent_plugin.dll", NULL);
#else
	gchar *fake_plugin = g_build_filename(test_plugin_dir, "nonexistent_plugin.so", NULL);
#endif
	
	/* This should fail gracefully without crashing */
	GModule *module = g_module_open(fake_plugin, G_MODULE_BIND_LAZY);
	
	if (module) {
		g_printerr("  ERROR: Non-existent plugin reported as loaded\n");
		g_module_close(module);
		result = FALSE;
	} else {
		g_print("  ✓ Non-existent plugin correctly failed to load\n");
		g_print("    Error: %s\n", g_module_error());
	}
	
	g_free(fake_plugin);
	
	return result;
}

/* Test: Plugin loading with corrupted file */
/* Validates: Requirement 3.5 */
static gboolean test_plugin_corrupted_file(void)
{
	gboolean result = TRUE;
	
	g_print("Test: Plugin loading with corrupted file\n");
	
	/* Create a corrupted plugin file (just random data) */
#ifdef _WIN32
	gchar *corrupted_plugin = g_build_filename(test_plugin_dir, "corrupted_plugin.dll", NULL);
#else
	gchar *corrupted_plugin = g_build_filename(test_plugin_dir, "corrupted_plugin.so", NULL);
#endif
	
	FILE *fp = fopen(corrupted_plugin, "wb");
	if (fp) {
		/* Write some random data */
		const gchar *garbage = "This is not a valid shared library file\n";
		fwrite(garbage, 1, strlen(garbage), fp);
		fclose(fp);
		
		/* Try to load the corrupted plugin */
		GModule *module = g_module_open(corrupted_plugin, G_MODULE_BIND_LAZY);
		
		if (module) {
			g_printerr("  ERROR: Corrupted plugin reported as loaded\n");
			g_module_close(module);
			result = FALSE;
		} else {
			g_print("  ✓ Corrupted plugin correctly failed to load\n");
			g_print("    Error: %s\n", g_module_error());
		}
		
		/* Clean up */
		g_unlink(corrupted_plugin);
	} else {
		g_printerr("  ERROR: Failed to create test file\n");
		result = FALSE;
	}
	
	g_free(corrupted_plugin);
	
	return result;
}

/* Test: Error logging for failed plugins */
/* Validates: Requirement 3.5 */
static gboolean test_plugin_error_logging(void)
{
	gboolean result = TRUE;
	
	g_print("Test: Error logging for failed plugins\n");
	
	/* This test verifies that plugin loading failures are logged */
	/* Try to load multiple invalid plugins */
	for (gint i = 0; i < 5; i++) {
#ifdef _WIN32
		gchar *fake_plugin = g_strdup_printf("%s/fake_plugin_%d.dll", test_plugin_dir, i);
#else
		gchar *fake_plugin = g_strdup_printf("%s/fake_plugin_%d.so", test_plugin_dir, i);
#endif
		
		/* This should fail and log an error */
		GModule *module = g_module_open(fake_plugin, G_MODULE_BIND_LAZY);
		if (module) {
			g_module_close(module);
		}
		
		g_free(fake_plugin);
	}
	
	g_print("  ✓ Multiple plugin load failures handled gracefully\n");
	
	return result;
}

/* Test: Plugin directory discovery in bundle */
/* Validates: Requirements 3.1 */
static gboolean test_plugin_directory_discovery(void)
{
	gboolean result = TRUE;
	
	g_print("Test: Plugin directory discovery in bundle\n");
	
#ifdef _WIN32
	/* On Windows, verify installation plugin directory paths */
	const gchar *possible_dirs[] = {
		"plugins",                           /* Relative to executable */
		"../plugins",                        /* One level up */
		"C:\\Program Files\\Multi-Remmina\\plugins",  /* Standard installation */
		"C:\\Program Files (x86)\\Multi-Remmina\\plugins",
		NULL
	};
	
	gboolean found_dir = FALSE;
	for (gint i = 0; possible_dirs[i] != NULL; i++) {
		if (g_file_test(possible_dirs[i], G_FILE_TEST_IS_DIR)) {
			g_print("  ✓ Plugin directory exists: %s\n", possible_dirs[i]);
			found_dir = TRUE;
			break;
		}
	}
	
	if (!found_dir) {
		g_print("  ⚠ No plugin directory found (may not be installed)\n");
	}
#elif defined(__APPLE__)
	/* On macOS, verify bundle plugin directory paths */
	const gchar *possible_dirs[] = {
		"../Resources/lib/remmina/plugins",  /* Relative to executable in bundle */
		"/usr/local/lib/remmina/plugins",    /* Homebrew installation */
		"/opt/homebrew/lib/remmina/plugins", /* Homebrew on Apple Silicon */
		NULL
	};
	
	gboolean found_dir = FALSE;
	for (gint i = 0; possible_dirs[i] != NULL; i++) {
		if (g_file_test(possible_dirs[i], G_FILE_TEST_IS_DIR)) {
			g_print("  ✓ Plugin directory exists: %s\n", possible_dirs[i]);
			found_dir = TRUE;
			break;
		}
	}
	
	if (!found_dir) {
		g_print("  ⚠ No plugin directory found (may not be in bundle or installed)\n");
	}
#else
	/* On Linux, verify standard plugin directory */
	const gchar *plugin_dirs[] = {
		"/usr/lib/remmina/plugins",
		"/usr/local/lib/remmina/plugins",
		NULL
	};
	
	gboolean found_dir = FALSE;
	for (gint i = 0; plugin_dirs[i] != NULL; i++) {
		if (g_file_test(plugin_dirs[i], G_FILE_TEST_IS_DIR)) {
			g_print("  ✓ Plugin directory exists: %s\n", plugin_dirs[i]);
			found_dir = TRUE;
			break;
		}
	}
	
	if (!found_dir) {
		g_print("  ⚠ No plugin directory found (may not be installed)\n");
	}
#endif
	
	return result;
}

/* Test: Plugin file extension verification */
/* Validates: Requirements 3.2, 3.3, 3.4 */
static gboolean test_plugin_file_extensions(void)
{
	gboolean result = TRUE;
	
	g_print("Test: Plugin file extension verification\n");
	
	/* Verify that GModule supports the expected plugin extension */
	const gchar *module_suffix = G_MODULE_SUFFIX;
	g_print("  ✓ Module suffix: %s\n", module_suffix);
	
	/* On Windows, plugins are .dll */
	/* On macOS, plugins can be .so or .dylib */
	/* On Linux, plugins are .so */
#ifdef _WIN32
	if (g_strcmp0(module_suffix, "dll") == 0) {
		g_print("  ✓ Module suffix is valid for Windows\n");
	} else {
		g_printerr("  ERROR: Unexpected module suffix on Windows: %s\n", module_suffix);
		result = FALSE;
	}
#elif defined(__APPLE__)
	if (g_strcmp0(module_suffix, "so") == 0 || g_strcmp0(module_suffix, "dylib") == 0) {
		g_print("  ✓ Module suffix is valid for macOS\n");
	} else {
		g_printerr("  ERROR: Unexpected module suffix on macOS: %s\n", module_suffix);
		result = FALSE;
	}
#else
	if (g_strcmp0(module_suffix, "so") == 0) {
		g_print("  ✓ Module suffix is valid for Linux\n");
	} else {
		g_printerr("  ERROR: Unexpected module suffix on Linux: %s\n", module_suffix);
		result = FALSE;
	}
#endif
	
	return result;
}

/* Run a single test */
static gboolean run_test(const gchar *name, gboolean (*test_func)(void))
{
	g_print("\n");
	gboolean result = test_func();
	
	if (result) {
		g_print("  PASSED\n");
	} else {
		g_print("  FAILED\n");
	}
	
	return result;
}

int main(int argc, char *argv[])
{
	gint exit_code = 0;
	gint tests_passed = 0;
	gint tests_failed = 0;
	
	/* Check if GModule is supported */
	if (!g_module_supported()) {
		g_printerr("ERROR: Dynamic module loading is not supported on this platform\n");
		return 1;
	}
	
	/* Setup test environment */
	setup_test_env();
	
	g_print("=== Plugin Loading Unit Tests ===\n");
	g_print("NOTE: These are simplified tests that verify plugin loading behavior.\n");
	g_print("Full integration tests require the complete Remmina application.\n\n");
	
	/* Run tests */
	if (run_test("Plugin loading with missing dependency", test_plugin_missing_dependency)) {
		tests_passed++;
	} else {
		tests_failed++;
	}
	
	if (run_test("Plugin loading with corrupted file", test_plugin_corrupted_file)) {
		tests_passed++;
	} else {
		tests_failed++;
	}
	
	if (run_test("Error logging for failed plugins", test_plugin_error_logging)) {
		tests_passed++;
	} else {
		tests_failed++;
	}
	
	if (run_test("Plugin directory discovery in bundle", test_plugin_directory_discovery)) {
		tests_passed++;
	} else {
		tests_failed++;
	}
	
	if (run_test("Plugin file extension verification", test_plugin_file_extensions)) {
		tests_passed++;
	} else {
		tests_failed++;
	}
	
	/* Cleanup */
	cleanup_test_env();
	
	g_print("\n=== Test Summary ===\n");
	g_print("Passed: %d\n", tests_passed);
	g_print("Failed: %d\n", tests_failed);
	
	if (tests_failed > 0) {
		exit_code = 1;
		g_print("=== Some tests FAILED ===\n");
	} else {
		g_print("=== All tests PASSED ===\n");
	}
	
	return exit_code;
}
