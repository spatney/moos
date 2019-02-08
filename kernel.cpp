#include "lib/types.h"
#include "lib/stdlib.h"
#include "lib/gdt.h";

extern "C" void boot(void * multiboot, uint32_t magic) {
    printf("Hello, I am your Operating System");

    GlobalDescriptorTable gdt;

    while(1);
}