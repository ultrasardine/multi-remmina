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

#pragma once

#ifdef __APPLE__

#include <glib.h>

G_BEGIN_DECLS

/**
 * remmina_keychain_macos_init:
 *
 * Initialize macOS Keychain support.
 *
 * Returns: TRUE if initialization succeeds, FALSE otherwise
 */
gboolean remmina_keychain_macos_init(void);

/**
 * remmina_keychain_macos_store_password:
 * @service: Service identifier (e.g., "Remmina")
 * @account: Account identifier (e.g., connection profile path)
 * @password: Password to store
 * @error: Return location for error, or NULL
 *
 * Store a password in macOS Keychain.
 *
 * Returns: TRUE if the password was stored successfully, FALSE otherwise
 */
gboolean remmina_keychain_macos_store_password(const gchar *service,
						const gchar *account,
						const gchar *password,
						GError **error);

/**
 * remmina_keychain_macos_get_password:
 * @service: Service identifier
 * @account: Account identifier
 * @error: Return location for error, or NULL
 *
 * Retrieve a password from macOS Keychain.
 *
 * Returns: The password string (must be freed with g_free), or NULL on error
 */
gchar* remmina_keychain_macos_get_password(const gchar *service,
					    const gchar *account,
					    GError **error);

/**
 * remmina_keychain_macos_delete_password:
 * @service: Service identifier
 * @account: Account identifier
 * @error: Return location for error, or NULL
 *
 * Delete a password from macOS Keychain.
 *
 * Returns: TRUE if the password was deleted successfully, FALSE otherwise
 */
gboolean remmina_keychain_macos_delete_password(const gchar *service,
						 const gchar *account,
						 GError **error);

G_END_DECLS

#endif /* __APPLE__ */
