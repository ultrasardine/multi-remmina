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

#include "remmina_keychain_macos.h"
#include <Security/Security.h>
#include <gio/gio.h>
#include <string.h>

/**
 * remmina_keychain_macos_init:
 *
 * Initialize macOS Keychain support.
 * Currently no initialization is required.
 *
 * Returns: TRUE
 */
gboolean remmina_keychain_macos_init(void)
{
	return TRUE;
}

/**
 * remmina_keychain_macos_store_password:
 * @service: Service identifier (e.g., "Remmina")
 * @account: Account identifier (e.g., connection profile path)
 * @password: Password to store
 * @error: Return location for error, or NULL
 *
 * Store a password in macOS Keychain using SecKeychainAddGenericPassword.
 * If a password already exists for the service/account combination,
 * it will be deleted and replaced with the new password.
 *
 * Returns: TRUE if the password was stored successfully, FALSE otherwise
 */
gboolean remmina_keychain_macos_store_password(const gchar *service,
						const gchar *account,
						const gchar *password,
						GError **error)
{
	OSStatus status;
	SecKeychainItemRef itemRef = NULL;

	if (!service || !account || !password) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
					     "Service, account, and password must not be NULL");
		}
		return FALSE;
	}

	/* Try to add the password to the keychain */
	status = SecKeychainAddGenericPassword(
		NULL,                           /* default keychain */
		strlen(service),                /* service name length */
		service,                        /* service name */
		strlen(account),                /* account name length */
		account,                        /* account name */
		strlen(password),               /* password length */
		password,                       /* password data */
		&itemRef                        /* item reference */
	);

	if (status == errSecDuplicateItem) {
		/* Item already exists, delete it and try again */
		status = SecKeychainFindGenericPassword(
			NULL,                   /* default keychain */
			strlen(service),        /* service name length */
			service,                /* service name */
			strlen(account),        /* account name length */
			account,                /* account name */
			NULL,                   /* password length (not needed) */
			NULL,                   /* password data (not needed) */
			&itemRef                /* item reference */
		);

		if (status == noErr && itemRef) {
			status = SecKeychainItemDelete(itemRef);
			CFRelease(itemRef);
			itemRef = NULL;

			if (status != noErr) {
				if (error) {
					*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
							     "Failed to delete existing keychain item: error code %d",
							     (int)status);
				}
				return FALSE;
			}

			/* Try to add again after deletion */
			status = SecKeychainAddGenericPassword(
				NULL,
				strlen(service),
				service,
				strlen(account),
				account,
				strlen(password),
				password,
				&itemRef
			);
		}
	}

	if (itemRef) {
		CFRelease(itemRef);
	}

	if (status != noErr) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to store password in keychain: error code %d",
					     (int)status);
		}
		return FALSE;
	}

	return TRUE;
}

/**
 * remmina_keychain_macos_get_password:
 * @service: Service identifier
 * @account: Account identifier
 * @error: Return location for error, or NULL
 *
 * Retrieve a password from macOS Keychain using SecKeychainFindGenericPassword.
 *
 * Returns: The password string (must be freed with g_free), or NULL on error
 */
gchar* remmina_keychain_macos_get_password(const gchar *service,
					    const gchar *account,
					    GError **error)
{
	OSStatus status;
	void *passwordData = NULL;
	UInt32 passwordLength = 0;
	gchar *password = NULL;

	if (!service || !account) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
					     "Service and account must not be NULL");
		}
		return NULL;
	}

	status = SecKeychainFindGenericPassword(
		NULL,                   /* default keychain */
		strlen(service),        /* service name length */
		service,                /* service name */
		strlen(account),        /* account name length */
		account,                /* account name */
		&passwordLength,        /* password length */
		&passwordData,          /* password data */
		NULL                    /* item reference (not needed) */
	);

	if (status == errSecItemNotFound) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
					     "Password not found in keychain");
		}
		return NULL;
	}

	if (status == errSecAuthFailed) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED,
					     "Authentication failed accessing keychain");
		}
		return NULL;
	}

	if (status != noErr) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to retrieve password from keychain: error code %d",
					     (int)status);
		}
		return NULL;
	}

	/* Copy password data to a GLib-allocated string */
	if (passwordData && passwordLength > 0) {
		password = g_malloc0(passwordLength + 1);
		memcpy(password, passwordData, passwordLength);
		password[passwordLength] = '\0';

		/* Free the password data returned by Keychain */
		SecKeychainItemFreeContent(NULL, passwordData);
	}

	return password;
}

/**
 * remmina_keychain_macos_delete_password:
 * @service: Service identifier
 * @account: Account identifier
 * @error: Return location for error, or NULL
 *
 * Delete a password from macOS Keychain using SecKeychainItemDelete.
 *
 * Returns: TRUE if the password was deleted successfully, FALSE otherwise
 */
gboolean remmina_keychain_macos_delete_password(const gchar *service,
						 const gchar *account,
						 GError **error)
{
	OSStatus status;
	SecKeychainItemRef itemRef = NULL;

	if (!service || !account) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
					     "Service and account must not be NULL");
		}
		return FALSE;
	}

	/* Find the keychain item */
	status = SecKeychainFindGenericPassword(
		NULL,                   /* default keychain */
		strlen(service),        /* service name length */
		service,                /* service name */
		strlen(account),        /* account name length */
		account,                /* account name */
		NULL,                   /* password length (not needed) */
		NULL,                   /* password data (not needed) */
		&itemRef                /* item reference */
	);

	if (status == errSecItemNotFound) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
					     "Password not found in keychain");
		}
		return FALSE;
	}

	if (status != noErr) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to find keychain item: error code %d",
					     (int)status);
		}
		return FALSE;
	}

	/* Delete the keychain item */
	status = SecKeychainItemDelete(itemRef);
	CFRelease(itemRef);

	if (status != noErr) {
		if (error) {
			*error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
					     "Failed to delete keychain item: error code %d",
					     (int)status);
		}
		return FALSE;
	}

	return TRUE;
}

#endif /* __APPLE__ */
