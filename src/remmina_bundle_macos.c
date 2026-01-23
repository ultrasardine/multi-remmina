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

#include <CoreFoundation/CoreFoundation.h>
#include <glib.h>
#include "remmina_bundle_macos.h"

/**
 * Get the bundle's Resources directory path using CoreFoundation.
 */
gchar* remmina_get_bundle_resource_path(void)
{
	CFBundleRef bundle;
	CFURLRef resourceURL;
	char path[PATH_MAX];
	gchar *result = NULL;

	bundle = CFBundleGetMainBundle();
	if (!bundle) {
		g_warning("Not running from a bundle");
		return NULL;
	}

	resourceURL = CFBundleCopyResourcesDirectoryURL(bundle);
	if (!resourceURL) {
		g_warning("Failed to get bundle resources directory URL");
		return NULL;
	}

	if (CFURLGetFileSystemRepresentation(resourceURL, TRUE, (UInt8*)path, PATH_MAX)) {
		result = g_strdup(path);
	} else {
		g_warning("Failed to convert bundle resource URL to path");
	}

	CFRelease(resourceURL);
	return result;
}

/**
 * Get the resource directory path (UI files, themes, etc.).
 */
gchar* remmina_get_resource_dir(void)
{
	static gchar *resource_dir = NULL;

	if (!resource_dir) {
		gchar *bundle_path = remmina_get_bundle_resource_path();
		if (bundle_path) {
			resource_dir = bundle_path;
		} else {
			/* Fallback to current directory if not in bundle */
			resource_dir = g_strdup(".");
		}
	}

	return g_strdup(resource_dir);
}

/**
 * Get the plugin directory path.
 */
gchar* remmina_get_plugin_dir(void)
{
	static gchar *plugin_dir = NULL;

	if (!plugin_dir) {
		gchar *bundle_path = remmina_get_bundle_resource_path();
		if (bundle_path) {
			plugin_dir = g_build_filename(bundle_path, "lib", "remmina", "plugins", NULL);
			g_free(bundle_path);
		} else {
			/* Fallback to standard location if not in bundle */
			plugin_dir = g_strdup("/usr/local/lib/remmina/plugins");
		}
	}

	return g_strdup(plugin_dir);
}

/**
 * Get the UI directory path.
 */
gchar* remmina_get_ui_dir(void)
{
	static gchar *ui_dir = NULL;

	if (!ui_dir) {
		gchar *bundle_path = remmina_get_bundle_resource_path();
		if (bundle_path) {
			ui_dir = g_build_filename(bundle_path, "ui", NULL);
			g_free(bundle_path);
		} else {
			/* Fallback to current directory if not in bundle */
			ui_dir = g_strdup("./data/ui");
		}
	}

	return g_strdup(ui_dir);
}

#endif /* __APPLE__ */
