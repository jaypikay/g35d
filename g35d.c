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

#include "g35.h"

#define DAEMON_NAME "G35"

static int doDaemon = 0;
static char *pid_file = "/var/run/g35d.pid";


void signal_handler(int sig)
{
    switch (sig) {
        case SIGHUP:
            syslog(LOG_INFO, "%s daemon reload configuration", DAEMON_NAME);
            break;
        case SIGTERM:
        case SIGINT:
            g35_destroy();
            syslog(LOG_INFO, "%s daemon shuttdown", DAEMON_NAME);
            exit(EXIT_SUCCESS);
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

int main(int argc, char **argv)
{
    pid_t pid;
    unsigned int keys[G35_KEYS_READ_LENGTH] = {0};
    int ret = 0;
    int i;

    struct option longopts[] = {
        {"daemon", 0, 0, 'd'},
        {0}
    };
    int longidx, opt;

    while ((opt = getopt_long(argc, argv, "d", longopts, &longidx)) != -1) {
        switch (opt) {
            case 'd':
                doDaemon = 1;
                break;
            default:
                fprintf(stderr, "opt: %d\n", opt);
                break;
        }
    }

    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(DAEMON_NAME, LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "%s daemon is starting", DAEMON_NAME);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    pid = getpid();
    if (doDaemon) {
        pid = daemonize();
        syslog(LOG_INFO, "%s daemon is running in background (PID = %d)",
                DAEMON_NAME, pid);
    }

    if (g35_init()) {
        syslog(LOG_ERR, "%s daemon failed to claim G35 HID interface",
                DAEMON_NAME);
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "%s daemon has initilised libg35", DAEMON_NAME);

    for (;;) {
        sleep(1);
    }

    //if ((ret = g35_keypressed(keys)) > 0) {
    //    fprintf(stderr, "g35_keypressed = %d\n", ret);
    //    for (i = 0; i < ret; i++) {
    //        fprintf(stderr, "key[%d] = %d\n", i, keys[i]);
    //    }
    //}

    g35_destroy();
    syslog(LOG_INFO, "%s daemon shuttdown", DAEMON_NAME);

    return 0;
}
