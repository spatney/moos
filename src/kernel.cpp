//#define GRAPHICS_MODE

#include <common/types.h>
#include <common/console.h>

#include <hardware/video.h>
#include <hardware/pci.h>
#include <hardware/interrupts.h>

#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

#ifdef GRAPHICS_MODE
#include <gui/graphics.h>
#include <gui/desktop.h>
#include <gui/window.h>
#endif

#include <memorymanagement.h>
#include <multitasking.h>
#include <gdt.h>
#include <multiboot.h>

using namespace moos;
using namespace moos::hardware;
using namespace moos::drivers;
using namespace moos::common;

#ifdef GRAPHICS_MODE
using namespace moos::gui;
#endif

#ifndef GRAPHICS_MODE
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
    int32_t x, y;

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
    void OnMouseMove(int32_t xOffset, int32_t yOffset)
    {
        xOffset /= 4;
        yOffset /= 4;

        invertVideoMemoryAt(x, y);
        x += xOffset;
        y += yOffset;

        if (x < 0)
            x = 0;
        if (x >= 80)
            x = 79;
        if (y < 0)
            y = 0;
        if (y >= 25)
            y = 24;

        invertVideoMemoryAt(x, y);
    }
    void invertVideoMemoryAt(int8_t x, int8_t y)
    {
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
    }
};

void taskA()
{
    while (true)
    {
        Console::Write("A\n");
    }
}

void taskB()
{
    while (true)
    {
        Console::Write("B\n");
    }
}

#endif
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernel_main(uint32_t multiBootInfoAddress, uint32_t magic)
{
    Console::Clear();
    Console::Write("Booting MoOS Kernel ...\n\n");

    GlobalDescriptorTable gdt;
    TaskManager taskManager;

    size_t heapSize = 10 * 1024 * 1024; // 10MB;
    Console::Write("Initializing heap of %d MB...\n", heapSize / (1024 * 1024));

    multiboot_info *multiboot = (multiboot_info_t *) multiBootInfoAddress;
    uint32_t padding = 10 * 1024;
    MemoryManager memoryManager(multiboot->mem_upper * 1024 - heapSize - padding, heapSize);

    MouseEventHandler *m1 = new MouseEventHandler();
    delete m1;
    m1 = new MouseEventHandler();

    // multi-tasking demo
    /*Task t1(&gdt, taskA);
    Task t2(&gdt, taskB);
    taskManager.AddTask(&t1);
    taskManager.AddTask(&t2);*/

    InterruptManager interruptManager(
        0x20,
        &gdt,
        &taskManager);

    Console::Write("Initializing driver manager ...\n");

    DriverManager driverManager;
    PeripheralComponentInterconnectController pciController;
    Console::Write("Loading PCI device drivers ...\n");
    pciController.SelectDrivers(&driverManager, &interruptManager);

    MouseEventHandler *handler;

#ifndef GRAPHICS_MODE
    PrintFKeyboardEventHandler *keyboardEventHandler = new PrintFKeyboardEventHandler();
    KeyboardDriver keyboard(&interruptManager, keyboardEventHandler);
    driverManager.AddDriver(&keyboard);

    MouseToCosole *mouseToConsole = new MouseToCosole();
    handler = mouseToConsole;
#endif

#ifdef GRAPHICS_MODE
    GraphicsContext gc;
    Desktop *desktop = new Desktop(320, 200, 0xFF, 0xFF, 0xFF);
    handler = desktop;

    Window win1(desktop, 10, 10, 20, 20, 0xA8, 0, 0);
    Window win2(desktop, 40, 40, 30, 30, 0, 0xA8, 0);
    Window win3(desktop, 100, 150, 20, 20, 0, 0, 0xA8);
    Window win4(desktop, 200, 150, 30, 30, 0xA8, 0xA8, 0);

    desktop->AddChildWidget(&win1);
    desktop->AddChildWidget(&win2);
    desktop->AddChildWidget(&win3);
    desktop->AddChildWidget(&win4);
#endif

    MouseDriver mouse(&interruptManager, handler);
    driverManager.AddDriver(&mouse);

    Console::Write("Activating driver manager ...\n");
    driverManager.ActivateAll();

    Console::Write("Activating interrupt manager ...\n");
    interruptManager.Activate();

    Console::Write("\n\nMoOS\a> ");

    while (1)
    {
#ifdef GRAPHICS_MODE
        desktop->Draw(&gc);
#endif
    }
}