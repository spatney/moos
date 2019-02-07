#include "src/types.h"
#include "src/stdlib.h"

extern "C" void boot(void * multiboot, uint32_t magic) {
    printf("Soon this will be an OS -> ");
    printf(itoa(magic, 16));

    while(1);
}