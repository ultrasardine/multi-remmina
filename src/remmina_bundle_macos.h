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
 * Get the bundle's Resources directory path.
 * @return The path to the bundle's Resources directory, or NULL if not in a bundle.
 *         The returned string should be freed with g_free().
 */
gchar* remmina_get_bundle_resource_path(void);

/**
 * Get the resource directory path (UI files, themes, etc.).
 * @return The path to the resource directory.
 *         The returned string should be freed with g_free().
 */
gchar* remmina_get_resource_dir(void);

/**
 * Get the plugin directory path.
 * @return The path to the plugin directory.
 *         The returned string should be freed with g_free().
 */
gchar* remmina_get_plugin_dir(void);

/**
 * Get the UI directory path.
 * @return The path to the UI directory.
 *         The returned string should be freed with g_free().
 */
gchar* remmina_get_ui_dir(void);

G_END_DECLS

#endif /* __APPLE__ */
