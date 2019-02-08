#include "lib/types.h"
#include "lib/stdlib.h"
#include "lib/gdt.h"

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void boot(void *multiboot, uint32_t magic)
{
    GlobalDescriptorTable gdt;

    printf("Hello, I am your Operating System. \n");
    printf("My name is PixelMite :) \n");

    while (1)
        ;
}