//#define GRAPHICS_MODE

#include <common/types.h>
#include <common/console.h>

#include <hardware/video.h>
#include <hardware/pci.h>
#include <hardware/interrupts.h>

#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <gui/terminal.h>

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
using namespace moos::gui;

#ifndef GRAPHICS_MODE

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
    Terminal *terminal = new Terminal(); 
    KeyboardDriver keyboard(&interruptManager, terminal);
    driverManager.AddDriver(&keyboard);
    handler = terminal;
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