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

#include "libg35.h"
#include "g35d.h"

#define DAEMON_NAME "G35"

static int doDaemon = 0;
static char *pid_file = "/var/run/g35d.pid";

pthread_t keypress_thread;


static void exit_g35d(int exit_code)
{
    pthread_join(keypress_thread, NULL);

    g35_uinput_destroy();
    g35_destroy();

    syslog(LOG_INFO, "%s daemon shuttdown", DAEMON_NAME);

    closelog();
    exit(exit_code);
}

void signal_handler(int sig)
{
    switch (sig) {
        case SIGHUP:
            syslog(LOG_INFO, "%s daemon reload configuration", DAEMON_NAME);
            break;
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
            if (keys[0] > 0)
                g35_uinput_write(keys);
        }
        usleep(40);
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    int ret;

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

    ret = g35_uinput_init("/dev/uinput");

    if (pthread_create(&keypress_thread, 0, keypress_event_thread, 0) != 0) {
        perror("pthread_create");
        syslog(LOG_ERR, "%s daemon pthread_create failed", DAEMON_NAME);
        exit_g35d(EXIT_FAILURE);
    }

    for (;;)
        pause();

    exit_g35d(EXIT_SUCCESS);
    return 0;
}
