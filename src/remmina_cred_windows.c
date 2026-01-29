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

#include "remmina_cred_windows.h"
#include <windows.h>
#include <wincred.h>
#include <gio/gio.h>
#include <string.h>

/**
 * remmina_cred_windows_init:
 *
 * Initialize Windows Credential Manager support.
 * Currently no initialization is required.
 *
 * Returns: TRUE
 */
gboolean remmina_cred_windows_init(void)
{
	return TRUE;
}

/**
 * Convert UTF-8 string to wide character string (UTF-16).
 * Returns newly allocated wide string that must be freed with g_free.
 */
static wchar_t* utf8_to_wide(const gchar *utf8_str)
{
	if (!utf8_str)
		return NULL;
	
	glong items_written;
	gunichar2 *utf16 = g_utf8_to_utf16(utf8_str, -1, NULL, &items_written, NULL);
	
	return (wchar_t*)utf16;
}

/**
 * Convert wide character string (UTF-16) to UTF-8 string.
 * Returns newly allocated UTF-8 string that must be freed with g_free.
 */
static gchar* wide_to_utf8(const wchar_t *wide_str)
{
	if (!wide_str)
		return NULL;
	
	return g_utf16_to_utf8((const gunichar2*)wide_str, -1, NULL, NULL, NULL);
}


/**
 * remmina_cred_windows_store_password:
 * @target_name: Target identifier (e.g., "MultiRemmina:profile_path")
 * @username: Username for the credential
 * @password: Password to store
 * @error: Return location for error, or NULL
 *
 * Store a password in Windows Credential Manager using CredWriteW.
 * If a credential already exists for the target name, it will be overwritten.
 *
 * Returns: TRUE if the password was stored successfully, FALSE otherwise
 */
gboolean remmina_cred_windows_store_password(const gchar *target_name,
					      const gchar *username,
					      const gchar *password,
					      GError **error)
{
	CREDENTIALW cred;
	wchar_t *wide_target = NULL;
	wchar_t *wide_username = NULL;
	DWORD win_error;
	gboolean result = FALSE;

	if (!target_name || !username || !password) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
					     "Target name, username, and password must not be NULL");
		}
		return FALSE;
	}

	/* Convert strings to wide characters */
	wide_target = utf8_to_wide(target_name);
	wide_username = utf8_to_wide(username);

	if (!wide_target || !wide_username) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to convert strings to wide characters");
		}
		goto cleanup;
	}

	/* Initialize credential structure */
	memset(&cred, 0, sizeof(CREDENTIALW));
	cred.Type = CRED_TYPE_GENERIC;
	cred.TargetName = wide_target;
	cred.UserName = wide_username;
	cred.CredentialBlobSize = (DWORD)(strlen(password) * sizeof(char));
	cred.CredentialBlob = (LPBYTE)password;
	cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

	/* Write credential to Credential Manager */
	if (!CredWriteW(&cred, 0)) {
		win_error = GetLastError();
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to store password in Credential Manager: error code %lu",
					     (unsigned long)win_error);
		}
		goto cleanup;
	}

	result = TRUE;

cleanup:
	g_free(wide_target);
	g_free(wide_username);

	return result;
}

/**
 * remmina_cred_windows_get_password:
 * @target_name: Target identifier
 * @error: Return location for error, or NULL
 *
 * Retrieve a password from Windows Credential Manager using CredReadW.
 *
 * Returns: The password string (must be freed with g_free), or NULL on error
 */
gchar* remmina_cred_windows_get_password(const gchar *target_name,
					  GError **error)
{
	PCREDENTIALW pcred = NULL;
	wchar_t *wide_target = NULL;
	gchar *password = NULL;
	DWORD win_error;

	if (!target_name) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
					     "Target name must not be NULL");
		}
		return NULL;
	}

	/* Convert target name to wide characters */
	wide_target = utf8_to_wide(target_name);
	if (!wide_target) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to convert target name to wide characters");
		}
		return NULL;
	}

	/* Read credential from Credential Manager */
	if (!CredReadW(wide_target, CRED_TYPE_GENERIC, 0, &pcred)) {
		win_error = GetLastError();
		
		if (win_error == ERROR_NOT_FOUND) {
			if (error) {
				*error = g_error_new(G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
						     "Credential not found in Credential Manager");
			}
		} else if (win_error == ERROR_NO_SUCH_LOGON_SESSION) {
			if (error) {
				*error = g_error_new(G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED,
						     "No logon session exists for Credential Manager access");
			}
		} else {
			if (error) {
				*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
						     "Failed to retrieve password from Credential Manager: error code %lu",
						     (unsigned long)win_error);
			}
		}
		
		g_free(wide_target);
		return NULL;
	}

	/* Copy password data to a GLib-allocated string */
	if (pcred->CredentialBlob && pcred->CredentialBlobSize > 0) {
		password = g_malloc0(pcred->CredentialBlobSize + 1);
		memcpy(password, pcred->CredentialBlob, pcred->CredentialBlobSize);
		password[pcred->CredentialBlobSize] = '\0';
	} else {
		/* Empty password case */
		password = g_strdup("");
	}

	/* Free the credential returned by Credential Manager */
	CredFree(pcred);
	g_free(wide_target);

	return password;
}

/**
 * remmina_cred_windows_delete_password:
 * @target_name: Target identifier
 * @error: Return location for error, or NULL
 *
 * Delete a password from Windows Credential Manager using CredDeleteW.
 *
 * Returns: TRUE if the password was deleted successfully, FALSE otherwise
 */
gboolean remmina_cred_windows_delete_password(const gchar *target_name,
					       GError **error)
{
	wchar_t *wide_target = NULL;
	DWORD win_error;
	gboolean result = FALSE;

	if (!target_name) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
					     "Target name must not be NULL");
		}
		return FALSE;
	}

	/* Convert target name to wide characters */
	wide_target = utf8_to_wide(target_name);
	if (!wide_target) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to convert target name to wide characters");
		}
		return FALSE;
	}

	/* Delete credential from Credential Manager */
	if (!CredDeleteW(wide_target, CRED_TYPE_GENERIC, 0)) {
		win_error = GetLastError();
		
		if (win_error == ERROR_NOT_FOUND) {
			if (error) {
				*error = g_error_new(G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
						     "Credential not found in Credential Manager");
			}
		} else {
			if (error) {
				*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
						     "Failed to delete credential from Credential Manager: error code %lu",
						     (unsigned long)win_error);
			}
		}
		
		g_free(wide_target);
		return FALSE;
	}

	g_free(wide_target);
	return TRUE;
}

#endif /* _WIN32 */
