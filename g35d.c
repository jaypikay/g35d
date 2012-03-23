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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/stat.h>
#include <pthread.h>
#include <confuse.h>

#include "libg35.h"
#include "g35d.h"
#include "g35config.h"

#define DAEMON_NAME "G35"

static char *config_filename = "../g35d.rc";

static int doDaemon = 0;
static char *pid_file = "/var/run/g35d.pid";
static char *uinput_dev = "/dev/uinput";

pthread_t keypress_thread;


static void exit_g35d(int exit_code)
{
    if (keypress_thread)
        pthread_join(keypress_thread, NULL);

    g35_uinput_destroy();
    g35_destroy();

    syslog(LOG_INFO, "daemon shuttdown");

    closelog();
    exit(exit_code);
}

void signal_handler(int sig)
{
    switch (sig) {
        case SIGHUP:
            syslog(LOG_INFO, "%s daemon reload configuration", DAEMON_NAME);
            read_config(config_filename);
            break;
        case SIGABRT:
        case SIGTERM:
        case SIGINT:
            exit_g35d(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "Unhandled signal (%d) %s\n", sig, strsignal(sig));
            break;
    }
}

static pid_t daemonize()
{
    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    /* Terminate parent process */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();
    if (sid < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return sid;
}

static void *keypress_event_thread()
{
    unsigned int keys[G35_KEYS_READ_LENGTH] = {0};
    int ret = 0;

    for (;;) {
        if ((ret = g35_keypressed(keys, 40)) > 0) {
            if (keys[0] > 0) {
                ret = g35_uinput_write(keys);
            }
        }
        usleep(40);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    pid_t pid;
    int ret;

    struct option longopts[] = {
        {"config", required_argument, 0, 'c'},
        {"daemon", no_argument      , 0, 'd'},
        {"uinput", required_argument, 0, 'u'},
        {0}
    };
    int longidx, opt;

    while ((opt = getopt_long(argc, argv, "c:du:", longopts,
                    &longidx)) != -1) {
        switch (opt) {
            case 'c':
                config_filename = optarg;
                break;
            case 'd':
                doDaemon = 1;
                break;
            case 'u':
                uinput_dev = optarg;
                break;
            default:
                fprintf(stderr, "opt: %d\n", opt);
                break;
        }
    }

    read_config(config_filename);

    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(DAEMON_NAME, LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "daemon is starting");

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    pid = getpid();
    if (doDaemon) {
        pid = daemonize();
        syslog(LOG_INFO, "daemon pid is %d", pid);
    }
    fprintf(stderr, "g35d pid %d\n", pid);

    if (g35_init()) {
        syslog(LOG_ERR, "daemon failed to claim G35 HID interface");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "successfully initilised libg35");

    ret = g35_uinput_init(uinput_dev);
    if (ret < 0) {
        syslog(LOG_ERR, "failed to open uinput device `%s'", uinput_dev);
        exit_g35d(EXIT_FAILURE);
    }

    if (pthread_create(&keypress_thread, 0, keypress_event_thread, 0) != 0) {
        perror("pthread_create");
        syslog(LOG_ERR, "pthread_create failed");
        exit_g35d(EXIT_FAILURE);
    }

    for (;;)
        pause();

    exit_g35d(EXIT_SUCCESS);
    return 0;
}
