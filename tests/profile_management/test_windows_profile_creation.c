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
 * Test: Verify profile creation works on Windows
 * Requirements: 10.1, 10.5
 */
static gboolean test_windows_profile_creation(void)
{
	RemminaFile *profile = NULL;
	gchar *datadir = NULL;
	gboolean result = FALSE;
	
	g_print("Test: Windows profile creation with all parameters\n");
	
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
	
	/* Ensure config directory is created if needed */
	if (!g_file_test(datadir, G_FILE_TEST_IS_DIR)) {
		g_print("  Creating data directory...\n");
		if (g_mkdir_with_parents(datadir, 0700) != 0) {
			g_printerr("  FAILED: Could not create data directory\n");
			g_free(datadir);
			return FALSE;
		}
	}
	
	/* Verify directory exists */
	if (!g_file_test(datadir, G_FILE_TEST_IS_DIR)) {
		g_printerr("  FAILED: Data directory does not exist after creation\n");
		g_free(datadir);
		return FALSE;
	}
	
	g_print("  Data directory exists and is accessible\n");
	
	/* Create a new profile */
	profile = remmina_file_new();
	if (!profile) {
		g_printerr("  FAILED: Could not create new profile\n");
		g_free(datadir);
		return FALSE;
	}
	
	/* Set profile parameters */
	remmina_file_set_string(profile, "name", "Windows Test Connection");
	remmina_file_set_string(profile, "protocol", "RDP");
	remmina_file_set_string(profile, "server", "192.168.1.100");
	remmina_file_set_string(profile, "username", "windowsuser");
	remmina_file_set_string(profile, "password", "testpassword123");
	remmina_file_set_string(profile, "group", "WindowsTestGroup");
	remmina_file_set_int(profile, "resolution_width", 1920);
	remmina_file_set_int(profile, "resolution_height", 1080);
	remmina_file_set_int(profile, "colordepth", 32);
	remmina_file_set_int(profile, "quality", 2);
	
	/* Verify all parameters were set correctly */
	if (g_strcmp0(remmina_file_get_string(profile, "name"), "Windows Test Connection") != 0) {
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
	
	if (g_strcmp0(remmina_file_get_string(profile, "username"), "windowsuser") != 0) {
		g_printerr("  FAILED: Username not set correctly\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(profile, "password"), "testpassword123") != 0) {
		g_printerr("  FAILED: Password not set correctly\n");
		goto cleanup;
	}
	
	if (g_strcmp0(remmina_file_get_string(profile, "group"), "WindowsTestGroup") != 0) {
		g_printerr("  FAILED: Group not set correctly\n");
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
	
	if (remmina_file_get_int(profile, "colordepth", 0) != 32) {
		g_printerr("  FAILED: Color depth not set correctly\n");
		goto cleanup;
	}
	
	if (remmina_file_get_int(profile, "quality", 0) != 2) {
		g_printerr("  FAILED: Quality not set correctly\n");
		goto cleanup;
	}
	
	g_print("  PASSED: All profile parameters set correctly\n");
	result = TRUE;
	
cleanup:
	if (profile) {
		remmina_file_free(profile);
	}
	g_free(datadir);
	
	return result;
}

int main(int argc, char *argv[])
{
	gint exit_code = 0;
	
	/* Initialize GTK (required for Remmina functions) */
	gtk_init(&argc, &argv);
	
	g_print("=== Windows Profile Creation Test ===\n\n");
	
	if (!test_windows_profile_creation()) {
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
