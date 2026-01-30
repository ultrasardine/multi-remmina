/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2011 Vic Lee
 * Copyright (C) 2014-2015 Antenore Gatta, Fabio Castelli, Giovanni Panozzo
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

#include "config.h"
#include "glibsecret_plugin.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib/gstdio.h>
#if defined(_WIN32)
#include "remmina_cred_windows.h"
#elif defined(__APPLE__)
#include "remmina_keychain_macos.h"
#else
#include <libsecret/secret.h>
#endif
#include <remmina/plugin.h>

static RemminaPluginService *remmina_plugin_service = NULL;
#define REMMINA_PLUGIN_DEBUG(fmt, ...) remmina_plugin_service->_remmina_debug(__func__, fmt, ##__VA_ARGS__)

#if !defined(__APPLE__) && !defined(_WIN32)
/* Linux-specific libsecret schema and service */
static SecretSchema remmina_file_secret_schema =
{ "org.remmina.Password", SECRET_SCHEMA_NONE,
  {
	  { "filename",	  SECRET_SCHEMA_ATTRIBUTE_STRING },
	  { "key",	  SECRET_SCHEMA_ATTRIBUTE_STRING },
	  { NULL,	  0 }
  } };


#ifdef LIBSECRET_VERSION_0_18
static SecretService* secretservice;
static SecretCollection* defaultcollection;
#endif
#endif /* !__APPLE__ && !_WIN32 */


static gboolean remmina_plugin_glibsecret_is_service_available(RemminaSecretPlugin* plugin)
{
#if defined(_WIN32)
	/* Windows Credential Manager is always available */
	return TRUE;
#elif defined(__APPLE__)
	return TRUE;
#else
#ifdef LIBSECRET_VERSION_0_18
	if (secretservice && defaultcollection)
		return TRUE;
	else
		return FALSE;
#else
	return FALSE;
#endif
#endif /* _WIN32 / __APPLE__ */
}

static void remmina_plugin_glibsecret_unlock_secret_service(RemminaSecretPlugin* plugin)
{
	TRACE_CALL(__func__);

#if defined(_WIN32)
	/* No unlock needed for Windows Credential Manager */
	(void)plugin;
	return;
#elif defined(__APPLE__)
	/* No unlock needed for macOS Keychain */
	(void)plugin;
	return;
#else
#ifdef LIBSECRET_VERSION_0_18

	GError *error = NULL;
	GList *objects, *ul;
	gchar* lbl;

	if (secretservice && defaultcollection) {
		if (secret_collection_get_locked(defaultcollection)) {
			lbl = secret_collection_get_label(defaultcollection);
			REMMINA_PLUGIN_DEBUG("Requesting unlock of the default '%s' collection", lbl);
			objects = g_list_append(NULL, defaultcollection);
			secret_service_unlock_sync(secretservice, objects, NULL, &ul, &error);
			g_list_free(objects);
			g_list_free(ul);
		}
	}
#endif
#endif /* _WIN32 / __APPLE__ */
	return;
}

static void remmina_plugin_glibsecret_store_password(RemminaSecretPlugin* plugin, RemminaFile *remminafile, const gchar *key, const gchar *password)
{
	TRACE_CALL(__func__);
	GError *r = NULL;
	const gchar *path;
	gchar *s;

	path = remmina_plugin_service->file_get_path(remminafile);
	
#if defined(_WIN32)
	/* Use Windows Credential Manager */
	s = g_strdup_printf("MultiRemmina:%s-%s", path, key);
	if (remmina_cred_windows_store_password(s, "MultiRemmina", password, &r)) {
		REMMINA_PLUGIN_DEBUG("Password \"%s\" saved for file %s\n", key, path);
	} else {
		REMMINA_PLUGIN_DEBUG("Password \"%s\" cannot be saved for file %s: %s\n", key, path, r ? r->message : "Unknown error");
		if (r)
			g_error_free(r);
	}
	g_free(s);
#elif defined(__APPLE__)
	/* Use macOS Keychain */
	s = g_strdup_printf("%s-%s", path, key);
	if (remmina_keychain_macos_store_password("Remmina", s, password, &r)) {
		REMMINA_PLUGIN_DEBUG("Password \"%s\" saved for file %s\n", key, path);
	} else {
		REMMINA_PLUGIN_DEBUG("Password \"%s\" cannot be saved for file %s: %s\n", key, path, r ? r->message : "Unknown error");
		if (r)
			g_error_free(r);
	}
	g_free(s);
#else
	/* Use libsecret on Linux */
	s = g_strdup_printf("Remmina: %s - %s", remmina_plugin_service->file_get_string(remminafile, "name"), key);
	secret_password_store_sync(&remmina_file_secret_schema, SECRET_COLLECTION_DEFAULT, s, password,
		NULL, &r, "filename", path, "key", key, NULL);
	g_free(s);
	if (r == NULL) {
		REMMINA_PLUGIN_DEBUG("Password \"%s\" saved for file %s\n", key, path);
	}else  {
		REMMINA_PLUGIN_DEBUG("Password \"%s\" cannot be saved for file %s\n", key, path);
		g_error_free(r);
	}
#endif /* _WIN32 / __APPLE__ */
}

static gchar*
remmina_plugin_glibsecret_get_password(RemminaSecretPlugin* plugin, RemminaFile *remminafile, const gchar *key)
{
	TRACE_CALL(__func__);
	GError *r = NULL;
	const gchar *path;
	gchar *password;
	gchar *p;
	gchar *s;

	path = remmina_plugin_service->file_get_path(remminafile);
	
#if defined(_WIN32)
	/* Use Windows Credential Manager */
	s = g_strdup_printf("MultiRemmina:%s-%s", path, key);
	password = remmina_cred_windows_get_password(s, &r);
	g_free(s);
	if (password) {
		return password;
	} else {
		REMMINA_PLUGIN_DEBUG("Password cannot be found for file %s\n", path);
		if (r)
			g_error_free(r);
		return NULL;
	}
#elif defined(__APPLE__)
	/* Use macOS Keychain */
	s = g_strdup_printf("%s-%s", path, key);
	password = remmina_keychain_macos_get_password("Remmina", s, &r);
	g_free(s);
	if (password) {
		return password;
	} else {
		REMMINA_PLUGIN_DEBUG("Password cannot be found for file %s\n", path);
		if (r)
			g_error_free(r);
		return NULL;
	}
#else
	/* Use libsecret on Linux */
	password = secret_password_lookup_sync(&remmina_file_secret_schema, NULL, &r, "filename", path, "key", key, NULL);
	if (r == NULL) {
		p = g_strdup(password);
		secret_password_free(password);
		return p;
	}else  {
		REMMINA_PLUGIN_DEBUG("Password cannot be found for file %s\n", path);
		return NULL;
	}
#endif /* _WIN32 / __APPLE__ */
}

static void remmina_plugin_glibsecret_delete_password(RemminaSecretPlugin* plugin, RemminaFile *remminafile, const gchar *key)
{
	TRACE_CALL(__func__);
	GError *r = NULL;
	const gchar *path;
	gchar *s;

	path = remmina_plugin_service->file_get_path(remminafile);
	
#if defined(_WIN32)
	/* Use Windows Credential Manager */
	s = g_strdup_printf("MultiRemmina:%s-%s", path, key);
	if (remmina_cred_windows_delete_password(s, &r)) {
		REMMINA_PLUGIN_DEBUG("password \"%s\" deleted for file %s", key, path);
	} else {
		REMMINA_PLUGIN_DEBUG("password \"%s\" cannot be deleted for file %s", key, path);
		if (r)
			g_error_free(r);
	}
	g_free(s);
#elif defined(__APPLE__)
	/* Use macOS Keychain */
	s = g_strdup_printf("%s-%s", path, key);
	if (remmina_keychain_macos_delete_password("Remmina", s, &r)) {
		REMMINA_PLUGIN_DEBUG("password \"%s\" deleted for file %s", key, path);
	} else {
		REMMINA_PLUGIN_DEBUG("password \"%s\" cannot be deleted for file %s", key, path);
		if (r)
			g_error_free(r);
	}
	g_free(s);
#else
	/* Use libsecret on Linux */
	secret_password_clear_sync(&remmina_file_secret_schema, NULL, &r, "filename", path, "key", key, NULL);
	if (r == NULL)
		REMMINA_PLUGIN_DEBUG("password \"%s\" deleted for file %s", key, path);
	else
		REMMINA_PLUGIN_DEBUG("password \"%s\" cannot be deleted for file %s", key, path);
#endif /* _WIN32 / __APPLE__ */
}

static gboolean remmina_plugin_glibsecret_init(RemminaSecretPlugin* plugin)
{
#if defined(_WIN32)
	/* Use Windows Credential Manager */
	return remmina_cred_windows_init();
#elif defined(__APPLE__)
	/* Use macOS Keychain */
	return remmina_keychain_macos_init();
#else
#ifdef LIBSECRET_VERSION_0_18
	GError *error;
	error = NULL;
	secretservice = secret_service_get_sync(SECRET_SERVICE_LOAD_COLLECTIONS, NULL, &error);
	if (error) {
		g_print("[glibsecret] unable to get secret service: %s\n", error->message);
		return FALSE;
	}
	if (secretservice == NULL) {
		g_print("[glibsecret] unable to get secret service: Unknown error.\n");
		return FALSE;
	}

	defaultcollection = secret_collection_for_alias_sync(secretservice, SECRET_COLLECTION_DEFAULT, SECRET_COLLECTION_NONE, NULL, &error);
	if (error) {
		g_print("[glibsecret] unable to get secret service default collection: %s\n", error->message);
		return FALSE;
	}

	remmina_plugin_glibsecret_unlock_secret_service(plugin);
	return TRUE;

#else
	g_print("Libsecret was too old during compilation, disabling secret service.\n");
	return FALSE;
#endif
#endif /* _WIN32 / __APPLE__ */
}

static RemminaSecretPlugin remmina_plugin_glibsecret =
{ REMMINA_PLUGIN_TYPE_SECRET,
  "glibsecret",
  N_("Secured password storage in the GNOME keyring"),
  NULL,
  VERSION,
  2000,
  remmina_plugin_glibsecret_init,
  remmina_plugin_glibsecret_is_service_available,
  remmina_plugin_glibsecret_store_password,
  remmina_plugin_glibsecret_get_password,
  remmina_plugin_glibsecret_delete_password
};

G_MODULE_EXPORT gboolean remmina_plugin_entry(RemminaPluginService *service);

gboolean
remmina_plugin_entry(RemminaPluginService *service)
{
	TRACE_CALL(__func__);

	/* This function should only register the secret plugin. No init action
	 * should be performed here. Initialization will be done later
	 * with remmina_plugin_xxx_init() . */

	remmina_plugin_service = service;

	if (!service->register_plugin((RemminaPlugin*)&remmina_plugin_glibsecret)) {
		return FALSE;
	}

	return TRUE;

}
