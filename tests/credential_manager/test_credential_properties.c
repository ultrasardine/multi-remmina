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
#include "../../src/remmina_cred_windows.h"

/* Simple property-based testing framework */
typedef struct {
	gchar *target_name;
	gchar *username;
	gchar *password;
} CredentialTestData;

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

/* Generate test data */
static CredentialTestData* generate_test_data(void)
{
	CredentialTestData *data = g_malloc0(sizeof(CredentialTestData));
	/* Prefix target name to avoid conflicts with real credentials */
	gchar *random_part = generate_random_string(5, 32);
	data->target_name = g_strdup_printf("MultiRemminaTest_%s", random_part);
	g_free(random_part);
	data->username = generate_random_string(5, 32);
	data->password = generate_random_string(8, 64);
	return data;
}

/* Free test data */
static void free_test_data(CredentialTestData *data)
{
	if (data) {
		g_free(data->target_name);
		g_free(data->username);
		g_free(data->password);
		g_free(data);
	}
}


/* Feature: windows-port, Property 1: Credential Manager Password Round-Trip */
/* Validates: Requirements 2.1, 2.2, 2.6 */
static gboolean prop_credential_roundtrip(void)
{
	CredentialTestData *data = generate_test_data();
	GError *error = NULL;
	gchar *retrieved_password = NULL;
	gboolean result = FALSE;
	
	/* Store password */
	if (!remmina_cred_windows_store_password(data->target_name, data->username, 
						  data->password, &error)) {
		g_printerr("Failed to store password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Retrieve password */
	retrieved_password = remmina_cred_windows_get_password(data->target_name, &error);
	if (!retrieved_password) {
		g_printerr("Failed to retrieve password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Compare passwords */
	if (g_strcmp0(data->password, retrieved_password) != 0) {
		g_printerr("Password mismatch: expected '%s', got '%s'\n", 
			   data->password, retrieved_password);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	/* Cleanup - delete the test password */
	remmina_cred_windows_delete_password(data->target_name, NULL);
	g_free(retrieved_password);
	free_test_data(data);
	
	return result;
}

/* Feature: windows-port, Property 2: Credential Manager Deletion Completeness */
/* Validates: Requirements 2.3, 2.5 */
static gboolean prop_credential_deletion(void)
{
	CredentialTestData *data = generate_test_data();
	GError *error = NULL;
	gchar *retrieved_password = NULL;
	gboolean result = FALSE;
	
	/* Store password */
	if (!remmina_cred_windows_store_password(data->target_name, data->username, 
						  data->password, &error)) {
		g_printerr("Failed to store password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Delete password */
	if (!remmina_cred_windows_delete_password(data->target_name, &error)) {
		g_printerr("Failed to delete password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Try to retrieve deleted password - should fail */
	retrieved_password = remmina_cred_windows_get_password(data->target_name, &error);
	if (retrieved_password) {
		g_printerr("Password still exists after deletion\n");
		g_free(retrieved_password);
		goto cleanup;
	}
	
	/* Verify we got an error (password not found) */
	if (!error) {
		g_printerr("Expected error after deletion, got none\n");
		goto cleanup;
	}
	
	g_error_free(error);
	result = TRUE;
	
cleanup:
	free_test_data(data);
	
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
	
	/* Initialize Credential Manager */
	if (!remmina_cred_windows_init()) {
		g_printerr("Failed to initialize Credential Manager\n");
		return 1;
	}
	
	g_print("=== Windows Credential Manager Property-Based Tests ===\n\n");
	
	/* Run Property 1: Credential Manager Password Round-Trip */
	if (!run_property_test("Property 1: Credential Manager Password Round-Trip", 
			       prop_credential_roundtrip, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	/* Run Property 2: Credential Manager Deletion Completeness */
	if (!run_property_test("Property 2: Credential Manager Deletion Completeness", 
			       prop_credential_deletion, 100)) {
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
	printf("Credential Manager property tests are only available on Windows\n");
	return 0;
}

#endif /* _WIN32 */
