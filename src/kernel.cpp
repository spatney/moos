#include <common/types.h>
#include <common/console.h>

#include <hardware/video.h>
#include <hardware/pci.h>
#include <hardware/interrupts.h>

#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>

#include <gdt.h>

using namespace moos;
using namespace moos::hardware;
using namespace moos::drivers;
using namespace moos::common;

class PrintFKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(int8_t c)
    {
        if (c == 0X0E)
        {
            Console::Backspace();
        }
        else
        {
            Console::Write("%c", c);
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

void DrawBlueScreen(VideoGraphicsArray *vga)
{
    uint32_t w = 320;
    uint32_t h = 200;

    vga->setMode(w, h, 8);
    vga->FillRectangle(0, 0, w, h, 0x00, 0x00, 0x00);
    vga->FillRectangle(w / 4, h / 4, 100, 100, 0x00, 0x00, 0xA8);
    vga->FillRectangle(w / 6, h / 6, 50, 70, 0x00, 0xA8, 0x00);
    vga->FillRectangle(w * 2 / 3, h * 1 / 2 - 20, 100, 50, 0xA8, 0x00, 0x00);
    vga->FillRectangle(0, h - 20, w, 20, 0x00, 0x00, 0xA8);
}

extern "C" void kernel_main(void *multiboot, uint32_t magic)
{
    GlobalDescriptorTable gdt;
    InterruptManager interruptManager(&gdt);

    Console::Clear();
    Console::Write("Booting MoOS Kernel ...\n\n");

    Console::Write("Initializing driver manager ...\n");

    DriverManager driverManager;
    PeripheralComponentInterconnectController pciController;
    pciController.SelectDrivers(&driverManager, &interruptManager);

    PrintFKeyboardEventHandler keyboardEventHandler;
    KeyboardDriver keyboard(&interruptManager, &keyboardEventHandler);
    driverManager.AddDriver(&keyboard);

    MouseToCosole mouseToConsole;
    MouseDriver mouse(&interruptManager, &mouseToConsole);
    driverManager.AddDriver(&mouse);

    VideoGraphicsArray vga;

    Console::Write("Activating driver manager ...\n");
    driverManager.ActivateAll();

    Console::Write("Activating interrupt manager ...\n");
    interruptManager.Activate();

    Console::Write("\n\nMoOS\a> ");

    DrawBlueScreen(&vga);

    while (1)
        ;
}