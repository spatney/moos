#include "lib/types.h"
#include "lib/stdlib.h"

extern "C" void boot(void * multiboot, uint32_t magic) {
    printf("Hello, I am your Operating System");

    while(1);
}