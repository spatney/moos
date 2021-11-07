#include "lib/types.h"
#include "lib/stdlib.h"
#include "lib/gdt.h"
#include "lib/interrupts.h"
#include "lib/keyboard.h"

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernel_main(void *multiboot, uint32_t magic)
{
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    KeyboardDriver keyboard(&interrupts);

    interrupts.Activate();

    printf("Hi, I am MoOS's Kernel! :)\nThe magic numbers are %d and %x", 7, magic);
    printf("\nAnother line? wow");

    while (1)
        ;
}