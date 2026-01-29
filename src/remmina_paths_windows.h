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
 * remmina_paths_windows_init:
 *
 * Initialize Windows path handling support.
 *
 * Returns: TRUE if initialization succeeds, FALSE otherwise
 */
gboolean remmina_paths_windows_init(void);

/**
 * remmina_paths_get_install_dir:
 *
 * Get the application's installation directory.
 * Returns the directory containing multi-remmina.exe.
 *
 * Returns: The path to the installation directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_install_dir(void);

/**
 * remmina_paths_get_config_dir:
 *
 * Get the user's config directory (%APPDATA%\multi-remmina).
 * Creates the directory if it doesn't exist.
 *
 * Returns: The path to the config directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_config_dir(void);

/**
 * remmina_paths_get_data_dir:
 *
 * Get the user's data directory (%LOCALAPPDATA%\multi-remmina).
 * Creates the directory if it doesn't exist.
 *
 * Returns: The path to the data directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_data_dir(void);

/**
 * remmina_paths_get_plugin_dir:
 *
 * Get the plugins directory (install_dir\plugins).
 *
 * Returns: The path to the plugin directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_plugin_dir(void);

/**
 * remmina_paths_get_ui_dir:
 *
 * Get the UI directory (install_dir\share\multi-remmina\ui).
 *
 * Returns: The path to the UI directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_ui_dir(void);

/**
 * remmina_paths_get_theme_dir:
 *
 * Get the themes directory (install_dir\share\multi-remmina\theme).
 *
 * Returns: The path to the theme directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_theme_dir(void);

/**
 * remmina_paths_normalize:
 * @path: The path to normalize
 *
 * Normalize a path by converting forward slashes to backslashes
 * and handling mixed path separators.
 *
 * Returns: The normalized path (must be freed with g_free),
 *          or NULL if path is NULL
 */
gchar* remmina_paths_normalize(const gchar *path);

/**
 * remmina_paths_expand_env:
 * @path: The path containing environment variables
 *
 * Expand Windows environment variables in a path.
 * Supports %APPDATA%, %LOCALAPPDATA%, %USERPROFILE%, etc.
 *
 * Returns: The expanded path (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_expand_env(const gchar *path);

G_END_DECLS

#endif /* _WIN32 */
