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

extern cfg_t *g35d_cfg;

extern cfg_bool_t isdaemon;
extern char *pid_filename;

extern int keymap_G1;
extern int keymap_G2;
extern int keymap_G3;
extern int keymap_VOLDN;
extern int keymap_VOLUP;

void read_config(const char *filename);

#endif /* _G35CONFIG_H_ */
