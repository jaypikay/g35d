#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "g35.h"

int main(int argc, char **argv)
{
    unsigned int keys[G35_KEYS_READ_LENGTH] = {0};
    int ret = 0;
    int i;

    if (g35_init()) {
        fprintf(stderr, "Not able to init libg35\n");
        exit(1);
    }
    fprintf(stderr, "libg35 initialised\n");

    if ((ret = g35_keypressed(keys)) > 0) {
        fprintf(stderr, "g35_keypressed = %d\n", ret);
        for (i = 0; i < ret; i++) {
            fprintf(stderr, "key[%d] = %d\n", i, keys[i]);
        }
    }

    g35_destroy();
    fprintf(stderr, "libg35 destroyed\n");

    return 0;
}
