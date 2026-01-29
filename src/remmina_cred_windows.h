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

#ifdef _WIN32

#include <glib.h>

G_BEGIN_DECLS

/**
 * remmina_cred_windows_init:
 *
 * Initialize Windows Credential Manager support.
 *
 * Returns: TRUE if initialization succeeds, FALSE otherwise
 */
gboolean remmina_cred_windows_init(void);

/**
 * remmina_cred_windows_store_password:
 * @target_name: Target identifier (e.g., "MultiRemmina:profile_path")
 * @username: Username for the credential
 * @password: Password to store
 * @error: Return location for error, or NULL
 *
 * Store a password in Windows Credential Manager using CredWriteW.
 *
 * Returns: TRUE if the password was stored successfully, FALSE otherwise
 */
gboolean remmina_cred_windows_store_password(const gchar *target_name,
					      const gchar *username,
					      const gchar *password,
					      GError **error);

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
					  GError **error);

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
					       GError **error);

G_END_DECLS

#endif /* _WIN32 */
