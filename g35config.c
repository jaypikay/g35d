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

#include <string.h>
#include <unistd.h>
#include <confuse.h>

#include "g35config.h"

cfg_t *g35d_cfg = 0;

cfg_bool_t isdaemon = cfg_false;
char *pid_filename = "/var/run/g35d.pid";

int keymap_G1 = 59;
int keymap_G2 = 60;
int keymap_G3 = 61;
int keymap_VOLDN = 114;
int keymap_VOLUP = 115;

static cfg_opt_t sec_opt_keymap[] = {
    CFG_SIMPLE_INT("G1", &keymap_G1),
    CFG_SIMPLE_INT("G2", &keymap_G2),
    CFG_SIMPLE_INT("G3", &keymap_G3),
    CFG_SIMPLE_INT("VOL_DOWN", &keymap_VOLDN),
    CFG_SIMPLE_INT("VOL_UP", &keymap_VOLUP),
    CFG_END()
};

cfg_opt_t opt_g35d[] = {
    CFG_SIMPLE_BOOL("daemon", &isdaemon),
    CFG_SIMPLE_STR("pidfile", &pid_filename),
    //CFG_SEC("keymap", sec_opt_keymap, CFGF_MULTI | CFGF_TITLE),
    CFG_END()
};


void read_config(const char *filename)
{
    int ret;

    cfg_free(g35d_cfg);
    
    g35d_cfg = cfg_init(opt_g35d, 0);
    cfg_parse(g35d_cfg, filename);
}
