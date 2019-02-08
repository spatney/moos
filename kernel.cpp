#include "lib/types.h"
#include "lib/stdlib.h"
#include "lib/gdt.h"

extern "C" void boot(void * multiboot, uint32_t magic) {
    GlobalDescriptorTable gdt;

    printf("Hello, I am your Operating System. \n");
    printf("My name is PixelMite :) \n");

    while(1);
}