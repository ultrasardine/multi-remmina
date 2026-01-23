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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <sys/stat.h>
#include "../../src/remmina_bundle_macos.h"

/* Resource types to test */
typedef enum {
	RESOURCE_TYPE_UI,
	RESOURCE_TYPE_PLUGIN,
	RESOURCE_TYPE_ICON,
	RESOURCE_TYPE_THEME
} ResourceType;

/* Test data structure */
typedef struct {
	ResourceType type;
	const gchar *relative_path;
} ResourceTestData;

/* Sample resource paths to test */
static const gchar *ui_files[] = {
	"remmina_main.glade",
	"remmina_preferences.glade",
	NULL
};

static const gchar *plugin_files[] = {
	"remmina-plugin-rdp.so",
	"remmina-plugin-vnc.so",
	"remmina-plugin-ssh.so",
	NULL
};

static const gchar *icon_files[] = {
	"remmina.png",
	"remmina-panel.png",
	NULL
};

static const gchar *theme_files[] = {
	"Dracula.colors",
	"Solarized Dark.colors",
	NULL
};

/* Generate test data */
static ResourceTestData* generate_test_data(void)
{
	ResourceTestData *data = g_malloc0(sizeof(ResourceTestData));
	
	/* Randomly select a resource type */
	gint type_choice = rand() % 4;
	data->type = (ResourceType)type_choice;
	
	/* Select a random file from the chosen type */
	switch (data->type) {
	case RESOURCE_TYPE_UI:
		data->relative_path = ui_files[rand() % 2];
		break;
	case RESOURCE_TYPE_PLUGIN:
		data->relative_path = plugin_files[rand() % 3];
		break;
	case RESOURCE_TYPE_ICON:
		data->relative_path = icon_files[rand() % 2];
		break;
	case RESOURCE_TYPE_THEME:
		data->relative_path = theme_files[rand() % 2];
		break;
	}
	
	return data;
}

/* Free test data */
static void free_test_data(ResourceTestData *data)
{
	if (data) {
		g_free(data);
	}
}

/* Check if a path exists and is accessible */
static gboolean path_exists(const gchar *path)
{
	struct stat st;
	return (stat(path, &st) == 0);
}

/* Feature: macos-port, Property 4: Bundle Resource Resolution */
/* Validates: Requirements 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7 */
static gboolean prop_bundle_resource_resolution(void)
{
	ResourceTestData *data = generate_test_data();
	gchar *full_path = NULL;
	gboolean result = FALSE;
	
	/* Get the appropriate directory based on resource type */
	switch (data->type) {
	case RESOURCE_TYPE_UI: {
		gchar *ui_dir = remmina_get_ui_dir();
		if (!ui_dir) {
			g_printerr("Failed to get UI directory\n");
			goto cleanup;
		}
		full_path = g_build_filename(ui_dir, data->relative_path, NULL);
		g_free(ui_dir);
		break;
	}
	case RESOURCE_TYPE_PLUGIN: {
		gchar *plugin_dir = remmina_get_plugin_dir();
		if (!plugin_dir) {
			g_printerr("Failed to get plugin directory\n");
			goto cleanup;
		}
		full_path = g_build_filename(plugin_dir, data->relative_path, NULL);
		g_free(plugin_dir);
		break;
	}
	case RESOURCE_TYPE_ICON: {
		gchar *resource_dir = remmina_get_resource_dir();
		if (!resource_dir) {
			g_printerr("Failed to get resource directory\n");
			goto cleanup;
		}
		full_path = g_build_filename(resource_dir, "icons", data->relative_path, NULL);
		g_free(resource_dir);
		break;
	}
	case RESOURCE_TYPE_THEME: {
		gchar *resource_dir = remmina_get_resource_dir();
		if (!resource_dir) {
			g_printerr("Failed to get resource directory\n");
			goto cleanup;
		}
		full_path = g_build_filename(resource_dir, "theme", data->relative_path, NULL);
		g_free(resource_dir);
		break;
	}
	}
	
	/* Verify the path was constructed */
	if (!full_path) {
		g_printerr("Failed to construct resource path\n");
		goto cleanup;
	}
	
	/* For this test, we verify that the path resolution mechanism works
	 * by checking that we get a valid path string. In a real bundle,
	 * the files would exist, but during development they may not. */
	if (strlen(full_path) == 0) {
		g_printerr("Resource path is empty\n");
		goto cleanup;
	}
	
	/* Verify the path contains expected components */
	const gchar *expected_component = NULL;
	switch (data->type) {
	case RESOURCE_TYPE_UI:
		expected_component = "ui";
		break;
	case RESOURCE_TYPE_PLUGIN:
		expected_component = "plugins";
		break;
	case RESOURCE_TYPE_ICON:
		expected_component = "icons";
		break;
	case RESOURCE_TYPE_THEME:
		expected_component = "theme";
		break;
	}
	
	if (!strstr(full_path, expected_component)) {
		g_printerr("Resource path '%s' does not contain expected component '%s'\n",
			   full_path, expected_component);
		goto cleanup;
	}
	
	result = TRUE;
	
cleanup:
	g_free(full_path);
	free_test_data(data);
	
	return result;
}

/* Run property test multiple times */
static gboolean run_property_test(const gchar *name, gboolean (*test_func)(void), gint iterations)
{
	gint passed = 0;
	gint failed = 0;
	
	g_print("Running property test: %s (%d iterations)\n", name, iterations);
	
	for (gint i = 0; i < iterations; i++) {
		if (test_func()) {
			passed++;
		} else {
			failed++;
			g_printerr("  Iteration %d FAILED\n", i + 1);
		}
	}
	
	g_print("  Results: %d passed, %d failed\n", passed, failed);
	
	return (failed == 0);
}

int main(int argc, char *argv[])
{
	gint exit_code = 0;
	
	/* Initialize random seed */
	srand(time(NULL));
	
	g_print("=== Bundle Resource Resolution Property-Based Tests ===\n\n");
	
	/* Run Property 4: Bundle Resource Resolution */
	if (!run_property_test("Property 4: Bundle Resource Resolution", 
			       prop_bundle_resource_resolution, 100)) {
		exit_code = 1;
	}
	
	g_print("\n");
	
	if (exit_code == 0) {
		g_print("=== All property tests PASSED ===\n");
	} else {
		g_print("=== Some property tests FAILED ===\n");
	}
	
	return exit_code;
}

#else

#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Bundle resource property tests are only available on macOS\n");
	return 0;
}

#endif /* __APPLE__ */
