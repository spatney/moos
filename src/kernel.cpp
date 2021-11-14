//#define GRAPHICS_MODE

#include <core/memory.h>
#include <core/multitasking.h>
#include <core/gdt.h>
#include <core/multiboot.h>

#include <common/types.h>
#include <common/console.h>
#include <common/linkedlist.h>

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

using namespace moos;
using namespace moos::core;
using namespace moos::hardware;
using namespace moos::drivers;
using namespace moos::common;
using namespace moos::gui;

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

void taskA();
void taskB();
void listdemo();

extern "C" void kernel_main(uint32_t multiBootInfoAddress, uint32_t magic)
{
    Console::Clear();
    Console::Write("Booting MoOS Kernel ...\n\n");

    GlobalDescriptorTable gdt;
    TaskManager taskManager;
    InterruptManager interruptManager(0x20, &gdt, &taskManager);

    size_t heapSize = 10 * 1024 * 1024; // 10MB;
    Console::Write("Initializing heap of %d MB...\n", heapSize / (1024 * 1024));
    multiboot_info *multiboot = (multiboot_info_t *)multiBootInfoAddress;
    uint32_t padding = 10 * 1024;
    MemoryManager memoryManager(multiboot->mem_upper * 1024 - heapSize - padding, heapSize);

    // multi-tasking demo
    /*Task t1(&gdt, taskA);
    Task t2(&gdt, taskB);
    taskManager.AddTask(&t1);
    taskManager.AddTask(&t2);*/

    Console::Write("Initializing driver manager ...\n");

    DriverManager driverManager;
    PeripheralComponentInterconnectController pciController;

    Console::Write("Loading PCI device drivers ...\n");
    pciController.SelectDrivers(&driverManager, &interruptManager);

    MouseEventHandler *mouseHandler;
    KeyboardEventHandler *keyboardHandler;

#ifndef GRAPHICS_MODE
    auto *terminal = new Terminal();
    mouseHandler = terminal;
    keyboardHandler = terminal;
#endif

#ifdef GRAPHICS_MODE
    GraphicsContext gc;
    auto *desktop = new Desktop(320, 200, 0xFF, 0xFF, 0xFF);
    mouseHandler = desktop;
    keyboardHandler = desktop;

    Window win1(desktop, 10, 10, 20, 20, 0xA8, 0, 0);
    Window win2(desktop, 40, 40, 30, 30, 0, 0xA8, 0);
    Window win3(desktop, 100, 150, 20, 20, 0, 0, 0xA8);
    Window win4(desktop, 200, 150, 30, 30, 0xA8, 0xA8, 0);

    desktop->AddChildWidget(&win1);
    desktop->AddChildWidget(&win2);
    desktop->AddChildWidget(&win3);
    desktop->AddChildWidget(&win4);
#endif

    auto *keyboard = new KeyboardDriver(&interruptManager, keyboardHandler);
    driverManager.AddDriver(keyboard);
    MouseDriver mouse(&interruptManager, mouseHandler);
    driverManager.AddDriver(&mouse);

    Console::Write("Activating driver manager ...\n");
    driverManager.ActivateAll();

    Console::Write("Activating interrupt manager ...\n");
    interruptManager.Activate();

    Console::Write("\n\nMoOS\a> ");

    listdemo();

    while (1)
    {
#ifdef GRAPHICS_MODE
        desktop->Draw(&gc);
#endif
    }
}

#define WAIT 50
void taskA()
{
    while (true)
    {
        Console::Write("A\n");
        Console::Sleep(WAIT);
        Console::Write("C\n");
        Console::Sleep(WAIT);
    }
}

void taskB()
{
    while (true)
    {
        Console::Write("B\n");
        Console::Sleep(WAIT);
        Console::Write("D\n");
        Console::Sleep(WAIT);
    }
}

struct Data
{
    int32_t d;
};

void printlist(LinkedList *list)
{
    int32_t counter = 0;

    Console::Write("HEAD => ");

    for (auto it = list->begin(), end = list->end(); it != end; ++it)
    {
        auto data = list->PeekAt(counter++);
        if (data == 0)
        {
            break;
        }
        Console::Write("%d => ", ((Data *)data)->d);
    }

    Console::Write("TAIL\n");
}

void *createDataPointer(int32_t n)
{
    auto *data = new Data();
    data->d = n;
    return data;
}

void listdemo()
{
    auto *list = new LinkedList();

    Console::Write("Linked List Demo \n");

    list->AddFirst(createDataPointer(2));
    list->AddFirst(createDataPointer(1));
    list->AddLast(createDataPointer(3));
    list->AddLast(createDataPointer(4));
    list->AddFirst(createDataPointer(0));

    printlist(list);

    list->RemoveLast();
    list->RemoveFirst();
    list->RemoveLast();
    list->RemoveFirst();

    printlist(list);
}