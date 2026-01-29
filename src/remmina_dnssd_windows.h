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
 * RemminaDnssdCallback:
 * @service_name: Human-readable name of the discovered service
 * @hostname: Resolved hostname or IP address
 * @port: Service port number
 * @user_data: User data passed to remmina_dnssd_windows_browse()
 *
 * Callback function invoked when a network service is discovered via DNS-SD.
 */
typedef void (*RemminaDnssdCallback)(
	const gchar *service_name,
	const gchar *hostname,
	guint16 port,
	gpointer user_data);

/**
 * remmina_dnssd_windows_init:
 *
 * Initialize the Windows DNS-SD service discovery system.
 * This function must be called before any other DNS-SD functions.
 *
 * Returns: TRUE if initialization succeeded, FALSE otherwise
 */
gboolean remmina_dnssd_windows_init(void);

/**
 * remmina_dnssd_windows_browse:
 * @service_type: The service type to browse for (e.g., "_rfb._tcp" for VNC, "_rdp._tcp" for RDP)
 * @callback: Function to call when services are discovered
 * @user_data: User data to pass to the callback
 *
 * Start browsing for network services of the specified type using Windows DNS-SD APIs.
 * The callback will be invoked for each discovered service.
 *
 * Returns: TRUE if browsing started successfully, FALSE otherwise
 */
gboolean remmina_dnssd_windows_browse(
	const gchar *service_type,
	RemminaDnssdCallback callback,
	gpointer user_data);

/**
 * remmina_dnssd_windows_stop:
 *
 * Stop all active service discovery operations and clean up resources.
 * After calling this function, remmina_dnssd_windows_init() must be
 * called again before browsing can resume.
 */
void remmina_dnssd_windows_stop(void);

/**
 * remmina_dnssd_windows_cleanup:
 *
 * Clean up all DNS-SD resources. This should be called when the
 * application is shutting down.
 */
void remmina_dnssd_windows_cleanup(void);

/**
 * remmina_dnssd_windows_is_available:
 *
 * Check if DNS-SD service discovery is available on this system.
 *
 * Returns: TRUE if DNS-SD is available, FALSE otherwise
 */
gboolean remmina_dnssd_windows_is_available(void);

G_END_DECLS

#endif /* _WIN32 */
