/* ncmpc (Ncurses MPD Client)
 * (c) 2004-2018 The Music Player Daemon Project
 * Project homepage: http://musicpd.org
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Bindings.hxx"
#include "command.hxx"
#include "KeyName.hxx"
#include "i18n.h"
#include "ncmpc_curses.h"
#include "util/Macros.hxx"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include <signal.h>
#include <unistd.h>

const char *
get_key_names(const KeyBinding *bindings, command_t command, bool all)
{
	const auto &b = bindings[size_t(command)];

	static char keystr[80];

	g_strlcpy(keystr, key2str(b.keys[0]), sizeof(keystr));
	if (!all)
		return keystr;

	for (unsigned j = 1; j < MAX_COMMAND_KEYS &&
		     b.keys[j] > 0; j++) {
		g_strlcat(keystr, " ", sizeof(keystr));
		g_strlcat(keystr, key2str(b.keys[j]), sizeof(keystr));
	}
	return keystr;
}

command_t
find_key_command(const KeyBinding *bindings, int key)
{
	assert(bindings != nullptr);
	assert(key != 0);

	for (size_t i = 0; i < size_t(CMD_NONE); ++i) {
		for (int j = 0; j < MAX_COMMAND_KEYS; j++)
			if (bindings[i].keys[j] == key)
				return command_t(i);
	}

	return CMD_NONE;
}

void
assign_keys(KeyBinding *bindings, command_t command,
	    const std::array<int, MAX_COMMAND_KEYS> &keys)
{
	auto &b = bindings[size_t(command)];
	b.keys = keys;
#ifndef NCMPC_MINI
	b.modified = true;
#endif
}

#ifndef NCMPC_MINI

bool
check_key_bindings(KeyBinding *bindings, char *buf, size_t bufsize)
{
	bool success = true;

	for (size_t i = 0; i < size_t(CMD_NONE); ++i)
		bindings[i].conflict = false;

	for (size_t i = 0; i < size_t(CMD_NONE); ++i) {
		int j;
		command_t cmd;

		for(j=0; j<MAX_COMMAND_KEYS; j++) {
			if (bindings[i].keys[j] &&
			    (cmd = find_key_command(bindings, bindings[i].keys[j])) != command_t(i)) {
				if (buf) {
					snprintf(buf, bufsize,
						 _("Key %s assigned to %s and %s"),
						 key2str(bindings[i].keys[j]),
						 get_key_command_name(command_t(i)),
						 get_key_command_name(cmd));
				} else {
					fprintf(stderr,
						_("Key %s assigned to %s and %s"),
						key2str(bindings[i].keys[j]),
						get_key_command_name(command_t(i)),
						get_key_command_name(cmd));
					fputc('\n', stderr);
				}
				bindings[i].conflict = true;
				bindings[size_t(cmd)].conflict = true;
				success = false;
			}
		}
	}

	return success;
}

bool
write_key_bindings(FILE *f, const KeyBinding *bindings, int flags)
{
	const auto *cmds = get_command_definitions();

	if (flags & KEYDEF_WRITE_HEADER)
		fprintf(f, "## Key bindings for ncmpc (generated by ncmpc)\n\n");

	for (size_t i = 0; i < size_t(CMD_NONE) && !ferror(f); ++i) {
		if (bindings[i].modified || flags & KEYDEF_WRITE_ALL) {
			fprintf(f, "## %s\n", cmds[i].description);
			if (flags & KEYDEF_COMMENT_ALL)
				fprintf(f, "#");
			fprintf(f, "key %s = ", cmds[i].name);
			for (int j = 0; j < MAX_COMMAND_KEYS; j++) {
				if (j && bindings[i].keys[j])
					fprintf(f, ",  ");
				if (!j || bindings[i].keys[j]) {
					if (bindings[i].keys[j] < 256 &&
					    (isalpha(bindings[i].keys[j]) || isdigit(bindings[i].keys[j])))
						fprintf(f, "\'%c\'", bindings[i].keys[j]);
					else
						fprintf(f, "%d", bindings[i].keys[j]);
				}
			}
			fprintf(f,"\n\n");
		}
	}

	return ferror(f) == 0;
}

#endif /* NCMPC_MINI */