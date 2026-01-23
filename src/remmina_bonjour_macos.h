/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2024-2024 The Remmina Project
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
 * RemminaServiceCallback:
 * @service_name: Human-readable name of the discovered service
 * @hostname: Resolved hostname or IP address
 * @port: Service port number
 * @user_data: User data passed to remmina_service_discovery_browse()
 *
 * Callback function invoked when a network service is discovered via Bonjour.
 */
typedef void (*RemminaServiceCallback)(
	const gchar *service_name,
	const gchar *hostname,
	guint16 port,
	gpointer user_data);

/**
 * remmina_service_discovery_init:
 *
 * Initialize the Bonjour service discovery system.
 * This function must be called before any other service discovery functions.
 *
 * Returns: TRUE if initialization succeeded, FALSE otherwise
 */
gboolean remmina_service_discovery_init(void);

/**
 * remmina_service_discovery_browse:
 * @service_type: The service type to browse for (e.g., "_rfb._tcp" for VNC, "_rdp._tcp" for RDP)
 * @callback: Function to call when services are discovered
 * @user_data: User data to pass to the callback
 *
 * Start browsing for network services of the specified type using Bonjour.
 * The callback will be invoked for each discovered service.
 *
 * Returns: TRUE if browsing started successfully, FALSE otherwise
 */
gboolean remmina_service_discovery_browse(
	const gchar *service_type,
	RemminaServiceCallback callback,
	gpointer user_data);

/**
 * remmina_service_discovery_stop:
 *
 * Stop all active service discovery operations and clean up resources.
 * After calling this function, remmina_service_discovery_init() must be
 * called again before browsing can resume.
 */
void remmina_service_discovery_stop(void);

G_END_DECLS

#endif /* __APPLE__ */
