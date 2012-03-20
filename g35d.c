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

                fprintf(stderr, "g35_uinput_write = %d\n", ret);
            }
        }
        usleep(40);
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    int ret;

    struct option longopts[] = {
        {"daemon", no_argument      , 0, 'd'},
        {"uinput", required_argument, 0, 'u'},
        {0}
    };
    int longidx, opt;

    while ((opt = getopt_long(argc, argv, "du:", longopts, &longidx)) != -1) {
        switch (opt) {
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
