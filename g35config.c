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

static cfg_opt_t sec_opt_keymap[] = {
    CFG_INT("G1", DEFAULT_G1_KEY, CFGF_NONE),
    CFG_INT("G2", DEFAULT_G2_KEY, CFGF_NONE),
    CFG_INT("G3", DEFAULT_G3_KEY, CFGF_NONE),
    CFG_INT("VOL_DOWN", DEFAULT_VOLDOWN_KEY, CFGF_NONE),
    CFG_INT("VOL_UP", DEFAULT_VOLUP_KEY, CFGF_NONE),
    CFG_END()
};

cfg_opt_t opt_g35d[] = {
    CFG_BOOL("daemon", DEFAULT_DAEMON, CFGF_NONE),
    CFG_STR("pidfile", DEFAULT_PIDFILE, CFGF_NONE),
    CFG_STR("uinput", DEFAULT_UINPUT, CFGF_NONE),
    CFG_STR("profile", 0, CFGF_NODEFAULT),
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

void read_keymap_profile(unsigned int *keymap, const char *profile)
{
    cfg_t *keyprofile = NULL;
    unsigned int i;

    if (profile) {
        for (i = 0; i < cfg_size(g35d_cfg, "keymap"); ++i) {
            keyprofile = cfg_getnsec(g35d_cfg, "keymap", i);
            if (keyprofile) {
                if (!strncmp(cfg_title(keyprofile), profile, 255)) {
                    keymap[0] = cfg_getint(keyprofile, "G1");
                    keymap[1] = cfg_getint(keyprofile, "G2");
                    keymap[2] = cfg_getint(keyprofile, "G3");
                    keymap[3] = cfg_getint(keyprofile, "VOL_DOWN");
                    keymap[4] = cfg_getint(keyprofile, "VOL_UP");
                    break;
                }
            }
        }
    }
}

void close_config()
{
    cfg_free(g35d_cfg);
}
