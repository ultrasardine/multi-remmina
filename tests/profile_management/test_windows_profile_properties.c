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
#include <time.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "remmina_file.h"
#include "remmina_file_manager.h"

#ifdef _WIN32
#include "remmina_paths_windows.h"
#include "remmina_cred_windows.h"
#endif

/* Test data directory */
static gchar *test_datadir = NULL;

/* Profile test data structure */
typedef struct {
	gchar *name;
	gchar *protocol;
	gchar *server;
	gchar *username;
	gchar *password;
	gchar *group;
	gint resolution_width;
	gint resolution_height;
} ProfileTestData;

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
#ifdef _WIN32
	/* On Windows, use LOCALAPPDATA environment variable */
	g_setenv("LOCALAPPDATA", test_datadir, TRUE);
	
	/* Ensure the multi-remmina subdirectory exists */
	gchar *remmina_dir = g_build_filename(test_datadir, "multi-remmina", NULL);
	g_mkdir_with_parents(remmina_dir, 0700);
	g_free(remmina_dir);
#else
	g_setenv("XDG_DATA_HOME", test_datadir, TRUE);
	
	/* Ensure the remmina subdirectory exists */
	gchar *remmina_dir = g_build_filename(test_datadir, "remmina", NULL);
	g_mkdir_with_parents(remmina_dir, 0700);
	g_free(remmina_dir);
#endif
	
	g_print("Test data directory: %s\n", test_datadir);
}

/* Cleanup test environment */
static void cleanup_test_env(void)
{
	if (test_datadir) {
		/* Remove all files in test directory recursively */
#ifdef _WIN32
		gchar *remmina_dir = g_build_filename(test_datadir, "multi-remmina", NULL);
#else
		gchar *remmina_dir = g_build_filename(test_datadir, "remmina", NULL);
#endif
		
		GDir *dir = g_dir_open(remmina_dir, 0, NULL);
		if (dir) {
			const gchar *name;
			while ((name = g_dir_read_name(dir)) != NULL) {
				gchar *filepath = g_build_filename(remmina_dir, name, NULL);
				g_unlink(filepath);
				g_free(filepath);
			}
			g_dir_close(dir);
		}
		g_rmdir(remmina_dir);
		g_free(remmina_dir);
		
		/* Remove the directory */
		g_rmdir(test_datadir);
		g_free(test_datadir);
		test_datadir = NULL;
	}
}

/* Generate random alphanumeric string */
static gchar* generate_random_string(gint min_len, gint max_len)
{
	static const gchar charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	gint len = min_len + (rand() % (max_len - min_len + 1));
	gchar *str = g_malloc0(len + 1);
	
	for (gint i = 0; i < len; i++) {
		str[i] = charset[rand() % (sizeof(charset) - 1)];
	}
	str[len] = '\0';
	
	return str;
}

/* Generate random protocol */
static gchar* generate_random_protocol(void)
{
	static const gchar *protocols[] = {"RDP", "VNC", "SSH", "SPICE", "X2GO"};
	gint index = rand() % (sizeof(protocols) / sizeof(protocols[0]));
	return g_strdup(protocols[index]);
}

/* Generate random IP address */
static gchar* generate_random_ip(void)
{
	return g_strdup_printf("%d.%d.%d.%d",
		1 + (rand() % 254),
		rand() % 256,
		rand() % 256,
		1 + (rand() % 254));
}

/* Generate test profile data */
static ProfileTestData* generate_profile_data(void)
{
	ProfileTestData *data = g_malloc0(sizeof(ProfileTestData));
	data->name = generate_random_string(5, 32);
	data->protocol = generate_random_protocol();
	data->server = generate_random_ip();
	data->username = generate_random_string(5, 16);
	data->password = generate_random_string(8, 32);
	data->group = generate_random_string(5, 16);
	data->resolution_width = 800 + (rand() % 2400);
	data->resolution_height = 600 + (rand() % 1800);
	return data;
}

/* Free test profile data */
static void free_profile_data(ProfileTestData *data)
{
	if (data) {
		g_free(data->name);
		g_free(data->protocol);
		g_free(data->server);
		g_free(data->username);
		g_free(data->password);
		g_free(data->group);
		g_free(data);
	}
}

/* Feature: windows-port, Property 5: Connection Profile Persistence Round-Trip */
/* Validates: Requirements 10.1, 10.2, 10.3, 10.4 */
static gboolean prop_profile_persistence_roundtrip(void)
{
	ProfileTestData *data = generate_profile_data();
	RemminaFile *profile = NULL;
	RemminaFile *loaded_profile = NULL;
	gchar *filename = NULL;
	gboolean result = FALSE;
	
	/* Create a new profile */
	profile = remmina_file_new();
	if (!profile) {
		g_printerr("Failed to create new profile\n");
		goto cleanup;
	}
	
	/* Set profile parameters */
	remmina_file_set_string(profile, "name", data->name);
	remmina_file_set_string(profile, "protocol", data->protocol);
	remmina_file_set_string(profile, "server", data->server);
	remmina_file_set_string(profile, "username", data->username);
	remmina_file_set_string(profile, "password", data->password);
	remmina_file_set_string(profile, "group", data->group);
	remmina_file_set_int(profile, "resolution_width", data->resolution_width);
	remmina_file_set_int(profile, "resolution_height", data->resolution_height);
	
	/* Generate filename and save */
	remmina_file_generate_filename(profile);
	filename = g_strdup(remmina_file_get_filename(profile));
	
	if (!filename) {
		g_printerr("Failed to generate filename\n");
		goto cleanup;
	}
	
	remmina_file_save(profile);
	
	/* Verify file was created */
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) {
		g_printerr("Profile file was not created: %s\n", filename);
		goto cleanup;
	}
	
	/* Load the profile */
	loaded_profile = remmina_file_load(filename);
	if (!loaded_profile) {
		g_printerr("Failed to load saved profile\n");
		goto cleanup;
	}
	
	/* Verify all fields match */
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "name"), data->name) != 0) {
		g_printerr("Name mismatch: expected '%s', got '%s'\n",
			data->name, remmina_file_get_string(loaded_profile, "name"));
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "protocol"), data->protocol) != 0) {
		g_printerr("Protocol mismatch: expected '%s', got '%s'\n",
			data->protocol, remmina_file_get_string(loaded_profile, "protocol"));
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "server"), data->server) != 0) {
		g_printerr("Server mismatch: expected '%s', got '%s'\n",
			data->server, remmina_file_get_string(loaded_profile, "server"));
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "username"), data->username) != 0) {
		g_printerr("Username mismatch: expected '%s', got '%s'\n",
			data->username, remmina_file_get_string(loaded_profile, "username"));
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "group"), data->group) != 0) {
		g_printerr("Group mismatch: expected '%s', got '%s'\n",
			data->group, remmina_file_get_string(loaded_profile, "group"));
		goto cleanup;
	}
	
	if (remmina_file_get_int(loaded_profile, "resolution_width", 0) != data->resolution_width) {
		g_printerr("Resolution width mismatch: expected %d, got %d\n",
			data->resolution_width, remmina_file_get_int(loaded_profile, "resolution_width", 0));
		goto cleanup;
	}
	
	if (remmina_file_get_int(loaded_profile, "resolution_height", 0) != data->resolution_height) {
		g_printerr("Resolution height mismatch: expected %d, got %d\n",
			data->resolution_height, remmina_file_get_int(loaded_profile, "resolution_height", 0));
		goto cleanup;
	}
	
	/* Note: Password verification is handled separately by Credential Manager tests on Windows
	 * and secret plugin tests on Linux. We don't verify password here to avoid
	 * duplicating those tests. */
	
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
	
	free_profile_data(data);
	
	return result;
}

/* Feature: windows-port, Property 6: Profile Listing Completeness */
/* Validates: Requirements 10.6 */
static gboolean prop_profile_listing_completeness(void)
{
	gint num_profiles = 3 + (rand() % 8); /* 3-10 profiles */
	gchar **filenames = g_malloc0(sizeof(gchar*) * (num_profiles + 1));
	GHashTable *created_names = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	gboolean result = FALSE;
	gint i;
	
	/* Create random profiles */
	for (i = 0; i < num_profiles; i++) {
		ProfileTestData *data = generate_profile_data();
		RemminaFile *profile = remmina_file_new();
		
		if (!profile) {
			g_printerr("Failed to create profile %d\n", i);
			goto cleanup;
		}
		
		remmina_file_set_string(profile, "name", data->name);
		remmina_file_set_string(profile, "protocol", data->protocol);
		remmina_file_set_string(profile, "server", data->server);
		
		remmina_file_generate_filename(profile);
		filenames[i] = g_strdup(remmina_file_get_filename(profile));
		
		remmina_file_save(profile);
		
		/* Track created profile names */
		g_hash_table_insert(created_names, g_strdup(data->name), GINT_TO_POINTER(1));
		
		remmina_file_free(profile);
		free_profile_data(data);
	}
	
	/* Get data directory */
	gchar *datadir = remmina_file_get_datadir();
	if (!datadir) {
		g_printerr("Failed to get data directory\n");
		goto cleanup;
	}
	
	/* List all profiles in directory */
	GDir *dir = g_dir_open(datadir, 0, NULL);
	if (!dir) {
		g_printerr("Failed to open data directory: %s\n", datadir);
		g_free(datadir);
		goto cleanup;
	}
	
	gint found_count = 0;
	const gchar *name;
	while ((name = g_dir_read_name(dir)) != NULL) {
		if (g_str_has_suffix(name, ".remmina")) {
			gchar *filepath = g_build_filename(datadir, name, NULL);
			RemminaFile *profile = remmina_file_load(filepath);
			
			if (profile) {
				const gchar *profile_name = remmina_file_get_string(profile, "name");
				if (profile_name && g_hash_table_contains(created_names, profile_name)) {
					found_count++;
				}
				remmina_file_free(profile);
			}
			
			g_free(filepath);
		}
	}
	
	g_dir_close(dir);
	g_free(datadir);
	
	/* Verify we found all created profiles */
	if (found_count != num_profiles) {
		g_printerr("Profile count mismatch: created %d, found %d\n",
			num_profiles, found_count);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	/* Clean up test files */
	for (i = 0; i < num_profiles && filenames[i]; i++) {
		g_unlink(filenames[i]);
		g_free(filenames[i]);
	}
	g_free(filenames);
	g_hash_table_destroy(created_names);
	
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
	srand(time(NULL));
	
	/* Initialize GTK (required for Remmina functions) */
	gtk_init(&argc, &argv);
	
	/* Setup test environment */
	setup_test_env();
	
#ifdef _WIN32
	/* Initialize Windows paths */
	if (!remmina_paths_windows_init()) {
		g_printerr("Failed to initialize Windows paths\n");
		cleanup_test_env();
		return 1;
	}
	
	/* Initialize Windows Credential Manager */
	if (!remmina_cred_windows_init()) {
		g_printerr("Failed to initialize Windows Credential Manager\n");
		cleanup_test_env();
		return 1;
	}
#endif
	
	g_print("=== Windows Profile Management Property-Based Tests ===\n\n");
	
	/* Run Property 5: Connection Profile Persistence Round-Trip */
	if (!run_property_test("Property 5: Connection Profile Persistence Round-Trip",
			       prop_profile_persistence_roundtrip, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Run Property 6: Profile Listing Completeness */
	if (!run_property_test("Property 6: Profile Listing Completeness",
			       prop_profile_listing_completeness, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Cleanup */
	cleanup_test_env();
	
	if (exit_code == 0) {
		g_print("=== All property tests PASSED ===\n");
	} else {
		g_print("=== Some property tests FAILED ===\n");
	}
	
	return exit_code;
}
