/*
 * Remmina - The GTK+ Remote Desktop Client
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

#include <glib.h>
#include "json-glib/json-glib.h"

G_BEGIN_DECLS

/* Plugin download functionality is not available in Multi-Remmina */
#define DOWNLOAD_URL "https://github.com/ultrasardine/multi-remmina"
#define LIST_URL "https://github.com/ultrasardine/multi-remmina"
/* Stats collection is disabled in Multi-Remmina */
#define PERIODIC_UPLOAD_URL "https://github.com/ultrasardine/multi-remmina"
#define INFO_REQUEST_URL "https://github.com/ultrasardine/multi-remmina"

#define RED_TEXT(str) g_markup_printf_escaped("<span color=\"red\">%s</span>", str)
#define GREEN_TEXT(str) g_markup_printf_escaped("<span color=\"green\">%s</span>", str)
#define GRAY_TEXT(str) g_markup_printf_escaped("<span color=\"gray\">%s</span>", str)

void remmina_curl_handshake(gpointer data);
void remmina_curl_compose_message(gchar* body, char* type, char* url, gpointer data);

G_END_DECLS
