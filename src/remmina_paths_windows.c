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

#include "remmina_paths_windows.h"
#include <windows.h>
#include <shlobj.h>
#include <gio/gio.h>
#include <string.h>

/* Cached paths for performance */
static gchar *cached_install_dir = NULL;
static gchar *cached_config_dir = NULL;
static gchar *cached_data_dir = NULL;

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
 * remmina_paths_windows_init:
 *
 * Initialize Windows path handling support.
 * Currently no initialization is required.
 *
 * Returns: TRUE
 */
gboolean remmina_paths_windows_init(void)
{
	return TRUE;
}

/**
 * remmina_paths_get_install_dir:
 *
 * Get the application's installation directory using GetModuleFileNameW.
 * Returns the directory containing multi-remmina.exe.
 *
 * Returns: The path to the installation directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_install_dir(void)
{
	if (cached_install_dir)
		return g_strdup(cached_install_dir);
	
	wchar_t path[MAX_PATH];
	DWORD len;
	gchar *utf8_path = NULL;
	gchar *dir_path = NULL;
	
	/* Get the full path to the executable */
	len = GetModuleFileNameW(NULL, path, MAX_PATH);
	if (len == 0 || len >= MAX_PATH) {
		g_warning("Failed to get module filename: error code %lu", 
			  (unsigned long)GetLastError());
		return NULL;
	}
	
	/* Convert to UTF-8 */
	utf8_path = wide_to_utf8(path);
	if (!utf8_path) {
		g_warning("Failed to convert module path to UTF-8");
		return NULL;
	}
	
	/* Get the directory part */
	dir_path = g_path_get_dirname(utf8_path);
	g_free(utf8_path);
	
	/* Cache the result */
	cached_install_dir = g_strdup(dir_path);
	
	return dir_path;
}

/**
 * Helper function to get a known folder path using SHGetKnownFolderPath.
 */
static gchar* get_known_folder_path(REFKNOWNFOLDERID folder_id)
{
	PWSTR path = NULL;
	gchar *utf8_path = NULL;
	HRESULT hr;
	
	hr = SHGetKnownFolderPath(folder_id, 0, NULL, &path);
	if (FAILED(hr)) {
		g_warning("Failed to get known folder path: HRESULT 0x%08lx", 
			  (unsigned long)hr);
		return NULL;
	}
	
	utf8_path = wide_to_utf8(path);
	CoTaskMemFree(path);
	
	return utf8_path;
}

/**
 * Helper function to ensure a directory exists.
 */
static gboolean ensure_directory_exists(const gchar *path)
{
	if (!path)
		return FALSE;
	
	if (g_file_test(path, G_FILE_TEST_IS_DIR))
		return TRUE;
	
	/* Create directory with parents */
	if (g_mkdir_with_parents(path, 0700) != 0) {
		g_warning("Failed to create directory: %s", path);
		return FALSE;
	}
	
	return TRUE;
}

/**
 * remmina_paths_get_config_dir:
 *
 * Get the user's config directory (%APPDATA%\multi-remmina).
 * Creates the directory if it doesn't exist.
 *
 * Returns: The path to the config directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_config_dir(void)
{
	if (cached_config_dir)
		return g_strdup(cached_config_dir);
	
	gchar *appdata_path = NULL;
	gchar *config_path = NULL;
	
	/* Get %APPDATA% path */
	appdata_path = get_known_folder_path(&FOLDERID_RoamingAppData);
	if (!appdata_path) {
		g_warning("Failed to get APPDATA path");
		return NULL;
	}
	
	/* Build config directory path */
	config_path = g_build_filename(appdata_path, "multi-remmina", NULL);
	g_free(appdata_path);
	
	/* Ensure directory exists */
	if (!ensure_directory_exists(config_path)) {
		g_free(config_path);
		return NULL;
	}
	
	/* Cache the result */
	cached_config_dir = g_strdup(config_path);
	
	return config_path;
}

/**
 * remmina_paths_get_data_dir:
 *
 * Get the user's data directory (%LOCALAPPDATA%\multi-remmina).
 * Creates the directory if it doesn't exist.
 *
 * Returns: The path to the data directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_data_dir(void)
{
	if (cached_data_dir)
		return g_strdup(cached_data_dir);
	
	gchar *localappdata_path = NULL;
	gchar *data_path = NULL;
	
	/* Get %LOCALAPPDATA% path */
	localappdata_path = get_known_folder_path(&FOLDERID_LocalAppData);
	if (!localappdata_path) {
		g_warning("Failed to get LOCALAPPDATA path");
		return NULL;
	}
	
	/* Build data directory path */
	data_path = g_build_filename(localappdata_path, "multi-remmina", NULL);
	g_free(localappdata_path);
	
	/* Ensure directory exists */
	if (!ensure_directory_exists(data_path)) {
		g_free(data_path);
		return NULL;
	}
	
	/* Cache the result */
	cached_data_dir = g_strdup(data_path);
	
	return data_path;
}


/**
 * remmina_paths_get_plugin_dir:
 *
 * Get the plugins directory (install_dir\plugins).
 *
 * Returns: The path to the plugin directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_plugin_dir(void)
{
	gchar *install_dir = remmina_paths_get_install_dir();
	gchar *plugin_dir = NULL;
	
	if (!install_dir)
		return NULL;
	
	plugin_dir = g_build_filename(install_dir, "plugins", NULL);
	g_free(install_dir);
	
	return plugin_dir;
}

/**
 * remmina_paths_get_ui_dir:
 *
 * Get the UI directory (install_dir\share\multi-remmina\ui).
 *
 * Returns: The path to the UI directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_ui_dir(void)
{
	gchar *install_dir = remmina_paths_get_install_dir();
	gchar *ui_dir = NULL;
	
	if (!install_dir)
		return NULL;
	
	ui_dir = g_build_filename(install_dir, "share", "multi-remmina", "ui", NULL);
	g_free(install_dir);
	
	return ui_dir;
}

/**
 * remmina_paths_get_theme_dir:
 *
 * Get the themes directory (install_dir\share\multi-remmina\theme).
 *
 * Returns: The path to the theme directory (must be freed with g_free),
 *          or NULL on error
 */
gchar* remmina_paths_get_theme_dir(void)
{
	gchar *install_dir = remmina_paths_get_install_dir();
	gchar *theme_dir = NULL;
	
	if (!install_dir)
		return NULL;
	
	theme_dir = g_build_filename(install_dir, "share", "multi-remmina", "theme", NULL);
	g_free(install_dir);
	
	return theme_dir;
}

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
gchar* remmina_paths_normalize(const gchar *path)
{
	if (!path)
		return NULL;
	
	gchar *normalized = g_strdup(path);
	gchar *p;
	
	/* Convert forward slashes to backslashes */
	for (p = normalized; *p; p++) {
		if (*p == '/')
			*p = '\\';
	}
	
	/* Remove duplicate backslashes (except for UNC paths) */
	gchar *result = g_malloc0(strlen(normalized) + 1);
	gchar *dest = result;
	gboolean prev_was_backslash = FALSE;
	gboolean is_unc = (normalized[0] == '\\' && normalized[1] == '\\');
	
	for (p = normalized; *p; p++) {
		if (*p == '\\') {
			/* For UNC paths, keep the first two backslashes */
			if (is_unc && (p - normalized) < 2) {
				*dest++ = *p;
				prev_was_backslash = TRUE;
			} else if (!prev_was_backslash) {
				*dest++ = *p;
				prev_was_backslash = TRUE;
			}
		} else {
			*dest++ = *p;
			prev_was_backslash = FALSE;
		}
	}
	*dest = '\0';
	
	g_free(normalized);
	return result;
}

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
gchar* remmina_paths_expand_env(const gchar *path)
{
	if (!path)
		return NULL;
	
	wchar_t *wide_path = NULL;
	wchar_t expanded[MAX_PATH * 2];
	gchar *result = NULL;
	DWORD len;
	
	/* Convert to wide string */
	wide_path = utf8_to_wide(path);
	if (!wide_path) {
		g_warning("Failed to convert path to wide string");
		return NULL;
	}
	
	/* Expand environment variables */
	len = ExpandEnvironmentStringsW(wide_path, expanded, MAX_PATH * 2);
	g_free(wide_path);
	
	if (len == 0) {
		g_warning("Failed to expand environment strings: error code %lu",
			  (unsigned long)GetLastError());
		return NULL;
	}
	
	if (len > MAX_PATH * 2) {
		g_warning("Expanded path too long");
		return NULL;
	}
	
	/* Convert back to UTF-8 */
	result = wide_to_utf8(expanded);
	if (!result) {
		g_warning("Failed to convert expanded path to UTF-8");
		return NULL;
	}
	
	return result;
}

#endif /* _WIN32 */
