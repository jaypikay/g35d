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
#include <linux/input.h>

#include "g35config.h"

cfg_t *g35d_cfg = 0;

cfg_opt_t sec_opt_keymap[] = {
    CFG_INT("G1", KEY_NEXTSONG, CFGF_NONE),
    CFG_INT("G2", KEY_PLAYPAUSE, CFGF_NONE),
    CFG_INT("G3", KEY_PREVIOUS, CFGF_NONE),
    CFG_INT("VOL_DOWN", KEY_VOLUMEDOWN, CFGF_NONE),
    CFG_INT("VOL_UP", KEY_VOLUMEUP, CFGF_NONE),
    CFG_END()
};

cfg_opt_t opt_g35d[] = {
    CFG_BOOL("daemon", cfg_false, CFGF_NONE),
    CFG_STR("pidfile", "/var/run/g35d.pid", CFGF_NONE),
    CFG_SEC("keymap", sec_opt_keymap, CFGF_MULTI | CFGF_TITLE),
    CFG_END()
};


int read_config(const char *filename)
{
    int ret;

    cfg_free(g35d_cfg);
    
    g35d_cfg = cfg_init(opt_g35d, CFGF_NOCASE);
    ret = cfg_parse(g35d_cfg, filename);

    return ret;
}
