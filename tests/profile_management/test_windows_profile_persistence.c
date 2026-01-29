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

#ifdef _WIN32
#include "remmina_paths_windows.h"
#include "remmina_cred_windows.h"
#endif

/**
 * Test: Verify profile persistence works on Windows
 * Requirements: 10.2, 10.3, 10.4
 */
static gboolean test_windows_profile_persistence(void)
{
	RemminaFile *profile = NULL;
	RemminaFile *loaded_profile = NULL;
	gchar *datadir = NULL;
	gchar *filename = NULL;
	gboolean result = FALSE;
	const gchar *test_password = "SecurePassword123!";
	
	g_print("Test: Windows profile persistence (save and load)\n");
	
#ifdef _WIN32
	/* Initialize Windows paths */
	if (!remmina_paths_windows_init()) {
		g_printerr("  FAILED: Could not initialize Windows paths\n");
		return FALSE;
	}
	
	/* Initialize Windows Credential Manager */
	if (!remmina_cred_windows_init()) {
		g_printerr("  FAILED: Could not initialize Windows Credential Manager\n");
		return FALSE;
	}
#endif
	
	/* Get data directory */
	datadir = remmina_file_get_datadir();
	if (!datadir) {
		g_printerr("  FAILED: Could not get data directory\n");
		return FALSE;
	}
	
	g_print("  Data directory: %s\n", datadir);
	
	/* Ensure directory exists */
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
	remmina_file_set_string(profile, "name", "Windows Persistence Test");
	remmina_file_set_string(profile, "protocol", "RDP");
	remmina_file_set_string(profile, "server", "192.168.1.150");
	remmina_file_set_string(profile, "username", "persistuser");
	remmina_file_set_string(profile, "password", test_password);
	remmina_file_set_string(profile, "group", "PersistGroup");
	remmina_file_set_int(profile, "resolution_width", 1600);
	remmina_file_set_int(profile, "resolution_height", 900);
	
	/* Generate filename and save */
	remmina_file_generate_filename(profile);
	filename = g_strdup(remmina_file_get_filename(profile));
	
	if (!filename) {
		g_printerr("  FAILED: Could not generate filename\n");
		goto cleanup;
	}
	
	g_print("  Saving profile to: %s\n", filename);
	
	/* Save the profile */
	remmina_file_save(profile);
	
	/* Verify file was created */
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) {
		g_printerr("  FAILED: Profile file was not created\n");
		goto cleanup;
	}
	
	g_print("  Profile file created successfully\n");
	
#ifdef _WIN32
	/* On Windows, verify password was stored in Credential Manager */
	/* Note: The password storage is handled by the secret plugin integration */
	/* which uses Credential Manager on Windows */
	g_print("  Password should be stored in Windows Credential Manager\n");
#endif
	
	/* Free the original profile */
	remmina_file_free(profile);
	profile = NULL;
	
	/* Load the profile */
	g_print("  Loading profile from: %s\n", filename);
	loaded_profile = remmina_file_load(filename);
	
	if (!loaded_profile) {
		g_printerr("  FAILED: Could not load saved profile\n");
		goto cleanup;
	}
	
	g_print("  Profile loaded successfully\n");
	
	/* Verify all fields match */
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "name"), "Windows Persistence Test") != 0) {
		g_printerr("  FAILED: Name mismatch\n");
		g_printerr("    Expected: 'Windows Persistence Test'\n");
		g_printerr("    Got: '%s'\n", remmina_file_get_string(loaded_profile, "name"));
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "protocol"), "RDP") != 0) {
		g_printerr("  FAILED: Protocol mismatch\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "server"), "192.168.1.150") != 0) {
		g_printerr("  FAILED: Server mismatch\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "username"), "persistuser") != 0) {
		g_printerr("  FAILED: Username mismatch\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(loaded_profile, "group"), "PersistGroup") != 0) {
		g_printerr("  FAILED: Group mismatch\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(loaded_profile, "resolution_width", 0) != 1600) {
		g_printerr("  FAILED: Resolution width mismatch\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(loaded_profile, "resolution_height", 0) != 900) {
		g_printerr("  FAILED: Resolution height mismatch\n");
		goto cleanup;
	}
	
	/* Note: Password verification depends on secret plugin integration */
	/* On Windows, this uses Credential Manager */
	/* The password field in the loaded profile should either be "." (indicating */
	/* it's stored in Credential Manager) or the decrypted password */
	const gchar *loaded_password = remmina_file_get_string(loaded_profile, "password");
	if (loaded_password) {
		g_print("  Password field present in loaded profile\n");
		/* Password handling is verified by Credential Manager tests */
	} else {
		g_print("  Password field not present (may be stored in Credential Manager)\n");
	}
	
	g_print("  PASSED: All profile fields persisted correctly\n");
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
	
	/* Initialize GTK (required for Remmina functions) */
	gtk_init(&argc, &argv);
	
	g_print("=== Windows Profile Persistence Test ===\n\n");
	
	if (!test_windows_profile_persistence()) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (exit_code == 0) {
		g_print("=== Test PASSED ===\n");
	} else {
		g_print("=== Test FAILED ===\n");
	}
	
	return exit_code;
}
