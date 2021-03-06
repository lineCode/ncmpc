/* ncmpc (Ncurses MPD Client)
 * (c) 2004-2020 The Music Player Daemon Project
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

#include "callbacks.hxx"
#include "screen_utils.hxx"
#include "screen_status.hxx"
#include "mpdclient.hxx"

#include <curses.h>

bool
mpdclient_auth_callback(struct mpdclient *c) noexcept
{
	auto *connection = c->GetConnection();
	if (connection == nullptr)
		return false;

	mpd_connection_clear_error(connection);

	const auto password = screen_read_password(nullptr);
	if (password.empty())
		return false;

	mpd_send_password(connection, password.c_str());

	if (!mpd_response_finish(connection)) {
		c->HandleAuthError();
		return false;
	}

	c->Update();
	return true;
}

void
mpdclient_error_callback(const char *message) noexcept
{
	screen_status_message(message);
	screen_bell();
	doupdate();
}
