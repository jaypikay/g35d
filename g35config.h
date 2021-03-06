/*
 *  G35 Daemon for Linux
 *  Copyright (C) 2012  Julian Knauer <jpk-at-goatpr0n.de>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _G35CONFIG_H_
#define _G35CONFIG_H_

#define DEFAULT_DAEMON          cfg_false
#define DEFAULT_PIDFILE         "/var/run/g35d.pid"
#define DEFAULT_UINPUT          "/dev/uinput"

#define DEFAULT_G1_KEY          KEY_NEXTSONG
#define DEFAULT_G2_KEY          KEY_PLAYPAUSE
#define DEFAULT_G3_KEY          KEY_PREVIOUSSONG
#define DEFAULT_VOLUP_KEY       KEY_VOLUMEDOWN
#define DEFAULT_VOLDOWN_KEY     KEY_VOLUMEUP

extern cfg_t *g35d_cfg;

int read_config(const char *filename);
void read_keymap_profile(unsigned int *keymap, const char *profile);
void close_config();

#endif /* _G35CONFIG_H_ */
