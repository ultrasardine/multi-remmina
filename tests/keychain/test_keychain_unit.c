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
#include "../../src/remmina_keychain_macos.h"

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

#define TEST_CLEANUP(service, account) \
	remmina_keychain_macos_delete_password(service, account, NULL);

/* Test: Empty password storage */
/* Validates: Requirements 2.4 */
static void test_empty_password(void)
{
	TEST_START("Empty password storage");
	
	const gchar *service = "RemminaTest";
	const gchar *account = "test_empty_password";
	const gchar *password = "";
	GError *error = NULL;
	gchar *retrieved = NULL;
	
	/* Store empty password */
	gboolean store_result = remmina_keychain_macos_store_password(
		service, account, password, &error);
	TEST_ASSERT(store_result, "Empty password stored successfully");
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
		error = NULL;
	}
	
	/* Retrieve empty password */
	retrieved = remmina_keychain_macos_get_password(service, account, &error);
	TEST_ASSERT(retrieved != NULL, "Empty password retrieved");
	TEST_ASSERT(retrieved && g_strcmp0(retrieved, password) == 0, 
		    "Empty password matches");
	
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
	}
	
	g_free(retrieved);
	TEST_CLEANUP(service, account);
}

/* Test: Special characters in passwords */
/* Validates: Requirements 2.4 */
static void test_special_characters(void)
{
	TEST_START("Special characters in passwords");
	
	const gchar *service = "RemminaTest";
	const gchar *account = "test_special_chars";
	const gchar *password = "p@ssw0rd!#$%^&*(){}[]<>?/\\|~`+=";
	GError *error = NULL;
	gchar *retrieved = NULL;
	
	/* Store password with special characters */
	gboolean store_result = remmina_keychain_macos_store_password(
		service, account, password, &error);
	TEST_ASSERT(store_result, "Password with special characters stored");
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
		error = NULL;
	}
	
	/* Retrieve password */
	retrieved = remmina_keychain_macos_get_password(service, account, &error);
	TEST_ASSERT(retrieved != NULL, "Password with special characters retrieved");
	TEST_ASSERT(retrieved && g_strcmp0(retrieved, password) == 0, 
		    "Special characters preserved");
	
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
	}
	
	g_free(retrieved);
	TEST_CLEANUP(service, account);
}

/* Test: Unicode characters in passwords */
/* Validates: Requirements 2.4 */
static void test_unicode_password(void)
{
	TEST_START("Unicode characters in passwords");
	
	const gchar *service = "RemminaTest";
	const gchar *account = "test_unicode";
	const gchar *password = "пароль密码🔐";
	GError *error = NULL;
	gchar *retrieved = NULL;
	
	/* Store password with Unicode */
	gboolean store_result = remmina_keychain_macos_store_password(
		service, account, password, &error);
	TEST_ASSERT(store_result, "Password with Unicode stored");
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
		error = NULL;
	}
	
	/* Retrieve password */
	retrieved = remmina_keychain_macos_get_password(service, account, &error);
	TEST_ASSERT(retrieved != NULL, "Password with Unicode retrieved");
	TEST_ASSERT(retrieved && g_strcmp0(retrieved, password) == 0, 
		    "Unicode characters preserved");
	
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
	}
	
	g_free(retrieved);
	TEST_CLEANUP(service, account);
}

/* Test: Non-existent password retrieval */
/* Validates: Requirements 2.5 */
static void test_nonexistent_password(void)
{
	TEST_START("Non-existent password retrieval");
	
	const gchar *service = "RemminaTest";
	const gchar *account = "nonexistent_account_12345";
	GError *error = NULL;
	gchar *retrieved = NULL;
	
	/* Try to retrieve non-existent password */
	retrieved = remmina_keychain_macos_get_password(service, account, &error);
	TEST_ASSERT(retrieved == NULL, "Non-existent password returns NULL");
	TEST_ASSERT(error != NULL, "Error is set for non-existent password");
	
	if (error) {
		g_error_free(error);
	}
}

/* Test: Delete non-existent password */
/* Validates: Requirements 2.5 */
static void test_delete_nonexistent(void)
{
	TEST_START("Delete non-existent password");
	
	const gchar *service = "RemminaTest";
	const gchar *account = "nonexistent_delete_12345";
	GError *error = NULL;
	
	/* Try to delete non-existent password */
	gboolean result = remmina_keychain_macos_delete_password(
		service, account, &error);
	TEST_ASSERT(!result, "Delete non-existent password returns FALSE");
	TEST_ASSERT(error != NULL, "Error is set for non-existent password");
	
	if (error) {
		g_error_free(error);
	}
}

/* Test: NULL parameter handling */
/* Validates: Requirements 2.4, 2.5 */
static void test_null_parameters(void)
{
	TEST_START("NULL parameter handling");
	
	GError *error = NULL;
	gboolean result;
	gchar *retrieved;
	
	/* Test store with NULL service */
	result = remmina_keychain_macos_store_password(NULL, "account", "password", &error);
	TEST_ASSERT(!result, "Store with NULL service returns FALSE");
	TEST_ASSERT(error != NULL, "Error is set for NULL service");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	/* Test store with NULL account */
	result = remmina_keychain_macos_store_password("service", NULL, "password", &error);
	TEST_ASSERT(!result, "Store with NULL account returns FALSE");
	TEST_ASSERT(error != NULL, "Error is set for NULL account");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	/* Test store with NULL password */
	result = remmina_keychain_macos_store_password("service", "account", NULL, &error);
	TEST_ASSERT(!result, "Store with NULL password returns FALSE");
	TEST_ASSERT(error != NULL, "Error is set for NULL password");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	/* Test get with NULL service */
	retrieved = remmina_keychain_macos_get_password(NULL, "account", &error);
	TEST_ASSERT(retrieved == NULL, "Get with NULL service returns NULL");
	TEST_ASSERT(error != NULL, "Error is set for NULL service");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	/* Test get with NULL account */
	retrieved = remmina_keychain_macos_get_password("service", NULL, &error);
	TEST_ASSERT(retrieved == NULL, "Get with NULL account returns NULL");
	TEST_ASSERT(error != NULL, "Error is set for NULL account");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	/* Test delete with NULL service */
	result = remmina_keychain_macos_delete_password(NULL, "account", &error);
	TEST_ASSERT(!result, "Delete with NULL service returns FALSE");
	TEST_ASSERT(error != NULL, "Error is set for NULL service");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
	
	/* Test delete with NULL account */
	result = remmina_keychain_macos_delete_password("service", NULL, &error);
	TEST_ASSERT(!result, "Delete with NULL account returns FALSE");
	TEST_ASSERT(error != NULL, "Error is set for NULL account");
	if (error) {
		g_error_free(error);
		error = NULL;
	}
}

/* Test: Very long password */
/* Validates: Requirements 2.4 */
static void test_long_password(void)
{
	TEST_START("Very long password");
	
	const gchar *service = "RemminaTest";
	const gchar *account = "test_long_password";
	GError *error = NULL;
	gchar *retrieved = NULL;
	
	/* Create a 1000-character password */
	gchar *password = g_malloc0(1001);
	for (gint i = 0; i < 1000; i++) {
		password[i] = 'a' + (i % 26);
	}
	password[1000] = '\0';
	
	/* Store long password */
	gboolean store_result = remmina_keychain_macos_store_password(
		service, account, password, &error);
	TEST_ASSERT(store_result, "Long password stored successfully");
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
		error = NULL;
	}
	
	/* Retrieve long password */
	retrieved = remmina_keychain_macos_get_password(service, account, &error);
	TEST_ASSERT(retrieved != NULL, "Long password retrieved");
	TEST_ASSERT(retrieved && g_strcmp0(retrieved, password) == 0, 
		    "Long password matches");
	
	if (error) {
		g_printerr("    Error: %s\n", error->message);
		g_error_free(error);
	}
	
	g_free(password);
	g_free(retrieved);
	TEST_CLEANUP(service, account);
}

int main(int argc, char *argv[])
{
	/* Initialize Keychain */
	if (!remmina_keychain_macos_init()) {
		g_printerr("Failed to initialize Keychain\n");
		return 1;
	}
	
	g_print("=== Keychain Unit Tests (Edge Cases) ===\n\n");
	
	/* Run all tests */
	test_empty_password();
	g_print("\n");
	
	test_special_characters();
	g_print("\n");
	
	test_unicode_password();
	g_print("\n");
	
	test_nonexistent_password();
	g_print("\n");
	
	test_delete_nonexistent();
	g_print("\n");
	
	test_null_parameters();
	g_print("\n");
	
	test_long_password();
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
	printf("Keychain unit tests are only available on macOS\n");
	return 0;
}

#endif /* __APPLE__ */
