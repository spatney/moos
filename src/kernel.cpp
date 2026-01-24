// #define GRAPHICS_MODE

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
#include <drivers/disk.h>

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
    // CP437 box drawing: C9=╔ CD=═ BB=╗ BA=║ C8=╚ BC=╝ CB=╦ CA=╩ B0=░
    Console::Print("\n");
    Console::SetColor(0x0B); // Cyan
    
    // Line 1: ╔═══╗  ╔═══╗
    Console::Print("   ");
    Console::PutCP437(0xC9); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xBB);
    Console::Print("  ");
    Console::PutCP437(0xC9); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xBB);
    Console::Print("\n");
    
    // Line 2: ║ ◆ ║  ║ ◆ ║   MoOS v1.0
    Console::Print("   ");
    Console::PutCP437(0xBA); Console::Print(" ");
    Console::SetColor(0x0E); Console::PutCP437(0x04); // Diamond
    Console::SetColor(0x0B); Console::Print(" "); Console::PutCP437(0xBA);
    Console::Print("  ");
    Console::PutCP437(0xBA); Console::Print(" ");
    Console::SetColor(0x0E); Console::PutCP437(0x04);
    Console::SetColor(0x0B); Console::Print(" "); Console::PutCP437(0xBA);
    Console::Print("   ");
    Console::SetColor(0x0F);
    Console::Print("MoOS v1.0\n");
    
    // Line 3: ╚═╦═╝  ╚═╦═╝   Modern Operating System
    Console::SetColor(0x0B);
    Console::Print("   ");
    Console::PutCP437(0xC8); Console::PutCP437(0xCD); Console::PutCP437(0xCB); Console::PutCP437(0xCD); Console::PutCP437(0xBC);
    Console::Print("  ");
    Console::PutCP437(0xC8); Console::PutCP437(0xCD); Console::PutCP437(0xCB); Console::PutCP437(0xCD); Console::PutCP437(0xBC);
    Console::Print("   ");
    Console::SetColor(0x07);
    Console::Print("Modern Operating System\n");
    
    // Line 4: ╔═╩════╩═╗
    Console::SetColor(0x0B);
    Console::Print("   ");
    Console::PutCP437(0xC9); Console::PutCP437(0xCD); Console::PutCP437(0xCA); Console::PutCP437(0xCD); Console::PutCP437(0xCD);
    Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCA); Console::PutCP437(0xCD); Console::PutCP437(0xBB);
    Console::Print("\n");
    
    // Line 5: ║ ░░░░░░ ║
    Console::Print("   ");
    Console::PutCP437(0xBA); Console::Print(" ");
    Console::SetColor(0x0F);
    Console::PutCP437(0xB0); Console::PutCP437(0xB0); Console::PutCP437(0xB0); Console::PutCP437(0xB0); Console::PutCP437(0xB0); Console::PutCP437(0xB0);
    Console::SetColor(0x0B);
    Console::Print(" "); Console::PutCP437(0xBA);
    Console::Print("\n");
    
    // Line 6: ╚════════╝
    Console::Print("   ");
    Console::PutCP437(0xC8); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCD);
    Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xCD); Console::PutCP437(0xBC);
    Console::Print("\n\n");
    
    Console::SetColor(0x08);
    Console::Print("   Type 'help' for available commands.\n\n");
    Console::SetColor(0x07);
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
    Console::Init();
    Console::Printf("Booting MoOS Kernel ...\n\n");

    GlobalDescriptorTable gdt;
    TaskManager taskManager;
    InterruptManager interruptManager(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interruptManager, 0x80);

    auto *multiboot = (multiboot_info_t *)multiBootInfoAddress;
    size_t memUpperInBytes = multiboot->mem_upper * 1024;
    size_t leftPadding = 1024 * 1024 * 10;
    size_t rightPadding = 10 * 1024;
    size_t heapSize = memUpperInBytes - leftPadding - rightPadding;

    Console::Printf("Initializing heap of %d MB...\n", heapSize / (1024 * 1024));

    MemoryManager memoryManager(leftPadding, heapSize);
    Console::Printf("Initializing driver manager ...\n");

    DriverManager driverManager;
    PeripheralComponentInterconnectController pciController;

    Console::Printf("Loading PCI device drivers ...\n");
    pciController.SelectDrivers(&driverManager, &interruptManager);

    // Initialize disk subsystem
    DiskManager::Initialize();

    MouseEventHandler *mouseHandler;
    KeyboardEventHandler *keyboardHandler;

#ifndef GRAPHICS_MODE
    auto keyboard = new KeyboardDriver(&interruptManager, nullptr);
    auto terminal = new Terminal(keyboard);
    keyboard->SetEventHandler(terminal);
    mouseHandler = 0;
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

    auto keyboard = new KeyboardDriver(&interruptManager, keyboardHandler);
#endif

    driverManager.AddDriver(keyboard);
    MouseDriver mouse(&interruptManager, mouseHandler);
    driverManager.AddDriver(&mouse);

    Console::Printf("Activating driver manager ...\n");
    driverManager.ActivateAll();

    Console::Printf("Activating interrupt manager ...\n");
    interruptManager.Activate();

#ifndef GRAPHICS_MODE
    Console::Clear();
    DrawLogo();
    terminal->Reset();
#endif

    while (kernel_running)
    {
#ifdef GRAPHICS_MODE
        desktop->Draw(&gc);
#endif
    }
    Console::Printf("\n\nMoOS kernel halted!");
}