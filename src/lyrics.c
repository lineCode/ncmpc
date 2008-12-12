/* ncmpc
 * Copyright (C) 2008 Max Kellermann <max@duempel.org>
 * This project's homepage is: http://www.musicpd.org
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "lyrics.h"
#include "../config.h"

#include <assert.h>

static struct plugin_list plugins;

struct lyrics_loader {
	struct plugin_cycle *plugin;
};

void lyrics_init(void)
{
	plugin_list_init(&plugins);
	plugin_list_load_directory(&plugins, LYRICS_PLUGIN_DIR);
}

void lyrics_deinit(void)
{
	plugin_list_deinit(&plugins);
}

struct lyrics_loader *
lyrics_load(const char *artist, const char *title,
	    plugin_callback_t callback, void *data)
{
	struct lyrics_loader *loader = g_new(struct lyrics_loader, 1);
	const char *args[3] = { artist, title, NULL };

	assert(artist != NULL);
	assert(title != NULL);

	loader->plugin = plugin_run(&plugins, args,
				    callback, data);

	return loader;
}

void
lyrics_free(struct lyrics_loader *loader)
{
	plugin_stop(loader->plugin);
	g_free(loader);
}
