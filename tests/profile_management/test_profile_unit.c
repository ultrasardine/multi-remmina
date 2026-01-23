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
#include <sys/stat.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "remmina_file.h"
#include "remmina_file_manager.h"

/* Test data directory */
static gchar *test_datadir = NULL;

/* Setup test environment */
static void setup_test_env(void)
{
	/* Create a temporary directory for test profiles */
	test_datadir = g_dir_make_tmp("remmina_test_XXXXXX", NULL);
	if (!test_datadir) {
		g_printerr("Failed to create test directory\n");
		exit(1);
	}
	
	/* Set the test directory as the data directory */
	g_setenv("XDG_DATA_HOME", test_datadir, TRUE);
	
	g_print("Test data directory: %s\n", test_datadir);
}

/* Cleanup test environment */
static void cleanup_test_env(void)
{
	if (test_datadir) {
		/* Remove all files in test directory */
		GDir *dir = g_dir_open(test_datadir, 0, NULL);
		if (dir) {
			const gchar *name;
			while ((name = g_dir_read_name(dir)) != NULL) {
				gchar *filepath = g_build_filename(test_datadir, name, NULL);
				g_unlink(filepath);
				g_free(filepath);
			}
			g_dir_close(dir);
		}
		
		/* Remove the directory */
		g_rmdir(test_datadir);
		g_free(test_datadir);
		test_datadir = NULL;
	}
}

/* Test: Profile creation with all parameters */
static gboolean test_profile_creation(void)
{
	RemminaFile *profile = NULL;
	gboolean result = FALSE;
	
	g_print("Test: Profile creation with all parameters\n");
	
	/* Create a new profile */
	profile = remmina_file_new();
	if (!profile) {
		g_printerr("  FAILED: Could not create new profile\n");
		return FALSE;
	}
	
	/* Set profile parameters */
	remmina_file_set_string(profile, "name", "Test Connection");
	remmina_file_set_string(profile, "protocol", "RDP");
	remmina_file_set_string(profile, "server", "192.168.1.100");
	remmina_file_set_string(profile, "username", "testuser");
	remmina_file_set_string(profile, "group", "TestGroup");
	remmina_file_set_int(profile, "resolution_width", 1920);
	remmina_file_set_int(profile, "resolution_height", 1080);
	
	/* Verify parameters were set */
	if (g_strcmp0(remmina_file_get_string(profile, "name"), "Test Connection") != 0) {
		g_printerr("  FAILED: Name not set correctly\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(profile, "protocol"), "RDP") != 0) {
		g_printerr("  FAILED: Protocol not set correctly\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(profile, "server"), "192.168.1.100") != 0) {
		g_printerr("  FAILED: Server not set correctly\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(profile, "username"), "testuser") != 0) {
		g_printerr("  FAILED: Username not set correctly\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(profile, "resolution_width", 0) != 1920) {
		g_printerr("  FAILED: Resolution width not set correctly\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(profile, "resolution_height", 0) != 1080) {
		g_printerr("  FAILED: Resolution height not set correctly\n");
		goto cleanup;
	}
	
	g_print("  PASSED\n");
	result = TRUE;
	
cleanup:
	if (profile) {
		remmina_file_free(profile);
	}
	
	return result;
}

/* Test: Config directory creation */
static gboolean test_config_directory_creation(void)
{
	gchar *datadir = NULL;
	gboolean result = FALSE;
	
	g_print("Test: Config directory creation\n");
	
	/* Get the data directory */
	datadir = remmina_file_get_datadir();
	if (!datadir) {
		g_printerr("  FAILED: Could not get data directory\n");
		return FALSE;
	}
	
	g_print("  Data directory: %s\n", datadir);
	
	/* Check if directory exists or can be created */
	if (!g_file_test(datadir, G_FILE_TEST_IS_DIR)) {
		/* Try to create it */
		if (g_mkdir_with_parents(datadir, 0700) != 0) {
			g_printerr("  FAILED: Could not create data directory\n");
			g_free(datadir);
			return FALSE;
		}
	}
	
	/* Verify directory exists and is writable */
	if (!g_file_test(datadir, G_FILE_TEST_IS_DIR)) {
		g_printerr("  FAILED: Data directory does not exist\n");
		goto cleanup;
	}
	
	/* Test write access by creating a test file */
	gchar *testfile = g_build_filename(datadir, ".test_write", NULL);
	if (!g_file_set_contents(testfile, "test", -1, NULL)) {
		g_printerr("  FAILED: Data directory is not writable\n");
		g_free(testfile);
		goto cleanup;
	}
	
	/* Clean up test file */
	g_unlink(testfile);
	g_free(testfile);
	
	g_print("  PASSED\n");
	result = TRUE;
	
cleanup:
	g_free(datadir);
	return result;
}

/* Test: Profile save failure handling */
static gboolean test_profile_save_failure(void)
{
	RemminaFile *profile = NULL;
	gboolean result = FALSE;
	
	g_print("Test: Profile save failure handling\n");
	
	/* Create a profile with invalid filename */
	profile = remmina_file_new();
	if (!profile) {
		g_printerr("  FAILED: Could not create new profile\n");
		return FALSE;
	}
	
	/* Set an invalid filename (directory that doesn't exist) */
	remmina_file_set_filename(profile, "/nonexistent/directory/profile.remmina");
	
	/* Try to save - this should fail gracefully */
	remmina_file_save(profile);
	
	/* If we get here without crashing, the test passes */
	g_print("  PASSED (graceful failure handling)\n");
	result = TRUE;
	
	remmina_file_free(profile);
	return result;
}

/* Test: Profile load failure handling */
static gboolean test_profile_load_failure(void)
{
	RemminaFile *profile = NULL;
	gboolean result = FALSE;
	
	g_print("Test: Profile load failure handling\n");
	
	/* Try to load a non-existent file */
	profile = remmina_file_load("/nonexistent/profile.remmina");
	
	/* Should return NULL for non-existent file */
	if (profile != NULL) {
		g_printerr("  FAILED: Expected NULL for non-existent file\n");
		remmina_file_free(profile);
		return FALSE;
	}
	
	g_print("  PASSED\n");
	result = TRUE;
	
	return result;
}

/* Test: Profile save and load */
static gboolean test_profile_save_load(void)
{
	RemminaFile *profile = NULL;
	RemminaFile *loaded_profile = NULL;
	gchar *datadir = NULL;
	gchar *filename = NULL;
	gboolean result = FALSE;
	
	g_print("Test: Profile save and load\n");
	
	/* Get data directory and ensure it exists */
	datadir = remmina_file_get_datadir();
	if (!datadir) {
		g_printerr("  FAILED: Could not get data directory\n");
		return FALSE;
	}
	
	if (!g_file_test(datadir, G_FILE_TEST_IS_DIR)) {
		if (g_mkdir_with_parents(datadir, 0700) != 0) {
			g_printerr("  FAILED: Could not create data directory\n");
			g_free(datadir);
			return FALSE;
		}
	}
	
	/* Create a new profile */
	profile = remmina_file_new();
	if (!profile) {
		g_printerr("  FAILED: Could not create new profile\n");
		g_free(datadir);
		return FALSE;
	}
	
	/* Set profile parameters */
	remmina_file_set_string(profile, "name", "Test Save Load");
	remmina_file_set_string(profile, "protocol", "VNC");
	remmina_file_set_string(profile, "server", "192.168.1.200");
	remmina_file_set_string(profile, "username", "saveloaduser");
	remmina_file_set_int(profile, "resolution_width", 1280);
	remmina_file_set_int(profile, "resolution_height", 720);
	
	/* Generate filename and save */
	remmina_file_generate_filename(profile);
	filename = g_strdup(remmina_file_get_filename(profile));
	
	if (!filename) {
		g_printerr("  FAILED: Could not generate filename\n");
		goto cleanup;
	}
	
	g_print("  Saving to: %s\n", filename);
	remmina_file_save(profile);
	
	/* Verify file was created */
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) {
		g_printerr("  FAILED: Profile file was not created\n");
		goto cleanup;
	}
	
	/* Load the profile */
	loaded_profile = remmina_file_load(filename);
	if (!loaded_profile) {
		g_printerr("  FAILED: Could not load saved profile\n");
		goto cleanup;
	}
	
	/* Verify loaded data matches */
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "name"), "Test Save Load") != 0) {
		g_printerr("  FAILED: Loaded name does not match\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "protocol"), "VNC") != 0) {
		g_printerr("  FAILED: Loaded protocol does not match\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "server"), "192.168.1.200") != 0) {
		g_printerr("  FAILED: Loaded server does not match\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "username"), "saveloaduser") != 0) {
		g_printerr("  FAILED: Loaded username does not match\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(loaded_profile, "resolution_width", 0) != 1280) {
		g_printerr("  FAILED: Loaded resolution width does not match\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(loaded_profile, "resolution_height", 0) != 720) {
		g_printerr("  FAILED: Loaded resolution height does not match\n");
		goto cleanup;
	}
	
	g_print("  PASSED\n");
	result = TRUE;
	
cleanup:
	/* Clean up test file */
	if (filename) {
		g_unlink(filename);
		g_free(filename);
	}
	
	if (profile) {
		remmina_file_free(profile);
	}
	
	if (loaded_profile) {
		remmina_file_free(loaded_profile);
	}
	
	g_free(datadir);
	return result;
}

int main(int argc, char *argv[])
{
	gint exit_code = 0;
	gint passed = 0;
	gint failed = 0;
	
	/* Initialize GTK (required for some Remmina functions) */
	gtk_init(&argc, &argv);
	
	/* Setup test environment */
	setup_test_env();
	
	g_print("=== Profile Management Unit Tests ===\n\n");
	
	/* Run tests */
	if (test_profile_creation()) {
		passed++;
	} else {
		failed++;
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (test_config_directory_creation()) {
		passed++;
	} else {
		failed++;
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (test_profile_save_failure()) {
		passed++;
	} else {
		failed++;
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (test_profile_load_failure()) {
		passed++;
	} else {
		failed++;
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (test_profile_save_load()) {
		passed++;
	} else {
		failed++;
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Cleanup */
	cleanup_test_env();
	
	/* Print summary */
	g_print("=== Test Summary ===\n");
	g_print("Passed: %d\n", passed);
	g_print("Failed: %d\n", failed);
	
	if (exit_code == 0) {
		g_print("=== All tests PASSED ===\n");
	} else {
		g_print("=== Some tests FAILED ===\n");
	}
	
	return exit_code;
}
