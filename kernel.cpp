#include <stdio.h>

void kernalMain(void * multiboot, unsigned int magic) {
    printf("Soon this will be an OS %d", magic);

    while(1);
}