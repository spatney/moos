#include "lib/types.h"
#include "lib/stdlib.h"
#include "lib/gdt.h"
#include "lib/interrupts.h"
#include "lib/keyboard.h"
#include "lib/mouse.h"
#include "lib/driver.h"
#include "lib/hardware.h"

class PrintFKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(int8_t c)
    {
        if (c == 0X0E)
        {
            backspace();
        }
        else
        {
            printf("%c", c);
        }
    }
};

class MouseToCosole : public MouseEventHandler
{
    int8_t x, y;

public:
    MouseToCosole()
    {
        x = 40;
        y = 12;
        invertVideoMemoryAt(x, y);
    }
    void OnActivate() {}
    void OnMouseDown(uint8_t button)
    {
        invertVideoMemoryAt(x, y);
    }
    void OnMouseUp(uint8_t button)
    {
        invertVideoMemoryAt(x, y);
    }
    void OnMouseMove(int32_t xOffset, int32_t yOffSet)
    {
        invertVideoMemoryAt(x, y);

        x += xOffset;
        if (x >= 80)
        {
            x = 79;
        }
        if (x < 0)
        {
            x = 0;
        }
        y += yOffSet;
        if (y >= 25)
        {
            y = 24;
        }
        if (y < 0)
        {
            y = 0;
        }

        invertVideoMemoryAt(x, y);
    }
    void invertVideoMemoryAt(int8_t x, int8_t y)
    {
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
    }
};

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

    printf("Booting MoOS Kernel ...\n\n");

    printf("Initializing driver manager ...\n");

    DriverManager driverManager;

    PrintFKeyboardEventHandler keyboardEventHandler;
    KeyboardDriver keyboard(&interrupts, &keyboardEventHandler);
    driverManager.AddDriver(&keyboard);

    MouseToCosole mouseToConsole;
    MouseDriver mouse(&interrupts, &mouseToConsole);
    driverManager.AddDriver(&mouse);

    printf("Activating driver manager ...\n");
    driverManager.ActivateAll();

    printf("Activating interrupts ...\n");
    interrupts.Activate();

    printf("\nOS boot complete!\n\nMoOS\a> ");
    while (1)
        ;
}