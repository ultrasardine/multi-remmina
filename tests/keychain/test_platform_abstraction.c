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
#include <glib.h>

#if defined(_WIN32)
#include "../../src/remmina_cred_windows.h"
#elif defined(__APPLE__)
#include "../../src/remmina_keychain_macos.h"
#else
/* For Linux, we would need to include libsecret headers, but for this test
 * we'll focus on the macOS and Windows implementations and document the expected behavior */
#endif

/* Feature: windows-port, Property 7: Platform Abstraction Functional Equivalence */
/* Validates: Requirements 6.4 */

typedef struct {
	gchar *service;
	gchar *account;
	gchar *password;
} TestData;

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
static TestData* generate_test_data(void)
{
	TestData *data = g_malloc0(sizeof(TestData));
	/* Prefix service name to avoid conflicts with real credentials */
	gchar *random_part = generate_random_string(5, 32);
	data->service = g_strdup_printf("MultiRemminaTest_%s", random_part);
	g_free(random_part);
	data->account = generate_random_string(5, 32);
	data->password = generate_random_string(8, 64);
	return data;
}

/* Free test data */
static void free_test_data(TestData *data)
{
	if (data) {
		g_free(data->service);
		g_free(data->account);
		g_free(data->password);
		g_free(data);
	}
}

#if defined(_WIN32)

/* Test password operations on Windows using Credential Manager */
static gboolean test_password_operations_windows(void)
{
	TestData *data = generate_test_data();
	GError *error = NULL;
	gchar *retrieved_password = NULL;
	gboolean result = FALSE;
	gchar *target_name = NULL;
	
	/* Create target name in the format used by the secret plugin */
	target_name = g_strdup_printf("MultiRemmina:%s-%s", data->service, data->account);
	
	/* Test 1: Store password */
	if (!remmina_cred_windows_store_password(target_name, "MultiRemmina", 
						  data->password, &error)) {
		g_printerr("Windows: Failed to store password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Test 2: Retrieve password */
	retrieved_password = remmina_cred_windows_get_password(target_name, &error);
	if (!retrieved_password) {
		g_printerr("Windows: Failed to retrieve password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Test 3: Verify password matches */
	if (g_strcmp0(data->password, retrieved_password) != 0) {
		g_printerr("Windows: Password mismatch\n");
		goto cleanup;
	}
	
	/* Test 4: Delete password */
	if (!remmina_cred_windows_delete_password(target_name, &error)) {
		g_printerr("Windows: Failed to delete password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Test 5: Verify password is deleted */
	g_free(retrieved_password);
	retrieved_password = remmina_cred_windows_get_password(target_name, &error);
	if (retrieved_password) {
		g_printerr("Windows: Password still exists after deletion\n");
		goto cleanup;
	}
	
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	result = TRUE;
	
cleanup:
	if (error) g_error_free(error);
	g_free(retrieved_password);
	g_free(target_name);
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
	
	g_print("=== Platform Abstraction Property-Based Tests (Windows) ===\n\n");
	
	/* Run Property 7: Platform Abstraction Functional Equivalence */
	if (!run_property_test("Property 7: Platform Abstraction Functional Equivalence", 
			       test_password_operations_windows, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (exit_code == 0) {
		g_print("=== All platform abstraction tests PASSED ===\n");
		g_print("\nNote: This test validates that password operations work correctly on Windows.\n");
		g_print("The same test should be run on macOS and Linux to verify cross-platform equivalence.\n");
	} else {
		g_print("=== Some platform abstraction tests FAILED ===\n");
	}
	
	return exit_code;
}

#elif defined(__APPLE__)

/* Test password operations on macOS using Keychain */
static gboolean test_password_operations_macos(void)
{
	TestData *data = generate_test_data();
	GError *error = NULL;
	gchar *retrieved_password = NULL;
	gboolean result = FALSE;
	
	/* Test 1: Store password */
	if (!remmina_keychain_macos_store_password(data->service, data->account, 
						    data->password, &error)) {
		g_printerr("macOS: Failed to store password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Test 2: Retrieve password */
	retrieved_password = remmina_keychain_macos_get_password(data->service, 
								  data->account, &error);
	if (!retrieved_password) {
		g_printerr("macOS: Failed to retrieve password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Test 3: Verify password matches */
	if (g_strcmp0(data->password, retrieved_password) != 0) {
		g_printerr("macOS: Password mismatch\n");
		goto cleanup;
	}
	
	/* Test 4: Delete password */
	if (!remmina_keychain_macos_delete_password(data->service, data->account, &error)) {
		g_printerr("macOS: Failed to delete password: %s\n", 
			   error ? error->message : "unknown error");
		if (error) g_error_free(error);
		goto cleanup;
	}
	
	/* Test 5: Verify password is deleted */
	g_free(retrieved_password);
	retrieved_password = remmina_keychain_macos_get_password(data->service, 
								  data->account, &error);
	if (retrieved_password) {
		g_printerr("macOS: Password still exists after deletion\n");
		goto cleanup;
	}
	
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	result = TRUE;
	
cleanup:
	if (error) g_error_free(error);
	g_free(retrieved_password);
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
	srand(time(NULL));
	
	/* Initialize Keychain */
	if (!remmina_keychain_macos_init()) {
		g_printerr("Failed to initialize Keychain\n");
		return 1;
	}
	
	g_print("=== Platform Abstraction Property-Based Tests (macOS) ===\n\n");
	
	/* Run Property 7: Platform Abstraction Functional Equivalence */
	if (!run_property_test("Property 7: Platform Abstraction Functional Equivalence", 
			       test_password_operations_macos, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (exit_code == 0) {
		g_print("=== All platform abstraction tests PASSED ===\n");
		g_print("\nNote: This test validates that password operations work correctly on macOS.\n");
		g_print("The same test should be run on Windows and Linux to verify cross-platform equivalence.\n");
	} else {
		g_print("=== Some platform abstraction tests FAILED ===\n");
	}
	
	return exit_code;
}

#else

/* For Linux, we would test libsecret operations here */
/* This is a placeholder that documents the expected behavior */

int main(int argc, char *argv[])
{
	printf("=== Platform Abstraction Property-Based Tests (Linux) ===\n\n");
	printf("Note: This test should validate that password operations work correctly on Linux\n");
	printf("using libsecret, with equivalent behavior to the macOS Keychain and Windows\n");
	printf("Credential Manager implementations.\n\n");
	printf("Expected behavior:\n");
	printf("1. Store password - should succeed\n");
	printf("2. Retrieve password - should return the same password\n");
	printf("3. Delete password - should succeed\n");
	printf("4. Retrieve deleted password - should fail with NOT_FOUND error\n\n");
	printf("To implement: Add libsecret test code similar to the macOS and Windows implementations.\n");
	
	return 0;
}

#endif /* _WIN32 / __APPLE__ */
