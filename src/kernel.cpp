//#define GRAPHICS_MODE

#include <ostest.h>

#include <core/multiboot.h>
#include <core/gdt.h>
#include <core/syscall.h>
#include <core/memory.h>
#include <common/types.h>
#include <common/console.h>
#include <hardware/pci.h>
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

static void DrawLogo()
{
    drivers::AdvancedTechnologyAttachment *selectedDisk = 0;

    drivers::AdvancedTechnologyAttachment ata0m(0x1F0, true);
    if (ata0m.Identify())
    {
        selectedDisk = &ata0m;
    }

    drivers::AdvancedTechnologyAttachment ata0s(0x1F0, false);
    if (ata0s.Identify())
    {
        selectedDisk = &ata0s;
    }

    if (selectedDisk == 0)
    {
        return;
    }

    auto contents = file_system::PartitionTable::ReadFileContents(selectedDisk, "LOGO");

    auto color = Console::SetColor(10);
    common::Console::Write("%s\n\n", contents);
    Console::SetColor(color);
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

bool kernel_running = true;
extern "C" void kernel_main(uint32_t multiBootInfoAddress, uint32_t magic)
{
    Console::Clear();
    Console::Write("Booting MoOS Kernel ...\n\n");

    GlobalDescriptorTable gdt;
    TaskManager taskManager;
    InterruptManager interruptManager(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interruptManager, 0x80);

    auto *multiboot = (multiboot_info_t *)multiBootInfoAddress;
    size_t memUpperInBytes = multiboot->mem_upper * 1024;
    size_t leftPadding = 1024 * 1024 * 10;
    size_t rightPadding = 10 * 1024;
    size_t heapSize = memUpperInBytes - leftPadding - rightPadding;

    Console::Write("Initializing heap of %d MB...\n", heapSize / (1024 * 1024));

    MemoryManager memoryManager(leftPadding, heapSize);
    Console::Write("Initializing driver manager ...\n");

    DriverManager driverManager;
    PeripheralComponentInterconnectController pciController;

    Console::Write("Loading PCI device drivers ...\n");
    pciController.SelectDrivers(&driverManager, &interruptManager);

    MouseEventHandler *mouseHandler;
    KeyboardEventHandler *keyboardHandler;

#ifndef GRAPHICS_MODE
    auto terminal = new Terminal();
    mouseHandler = 0; // terminal;
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

    auto keyboard = new KeyboardDriver(&interruptManager, keyboardHandler);
    driverManager.AddDriver(keyboard);
    MouseDriver mouse(&interruptManager, mouseHandler);
    driverManager.AddDriver(&mouse);

    Console::Write("Activating driver manager ...\n");
    driverManager.ActivateAll();

    Console::Write("Activating interrupt manager ...\n");
    interruptManager.Activate();

#ifndef GRAPHICS_MODE
    Console::Clear();
    DrawLogo();
    // Console::Write("Welcome to MoOS!\n\n");
    //  OSTest::MultiTaskingTest(&taskManager, &gdt);
    //  OSTest::SysCallTest();
    //  OSTest::HardDiskTest();
    //  OSTest::SharedPtrDemo();
    //  OSTest::HeapDemo();
    //  OSTest::SleepDemo();
    //  OSTest::NetworkCardDemo(&driverManager);
    terminal->Reset();
#endif

    while (kernel_running)
    {
#ifdef GRAPHICS_MODE
        desktop->Draw(&gc);
#endif
    }
    Console::Write("\n\nMoOS kernal halted!");
}