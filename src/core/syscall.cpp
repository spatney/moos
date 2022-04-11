#include <core/syscall.h>
#include <common/console.h>

using namespace moos::core;
using namespace moos::hardware;
using namespace moos::common;

extern "C" bool kernal_running;

SyscallHandler::SyscallHandler(
    InterruptManager *interruptManager,
    uint8_t interruptNumber)
    : InterruptHandler(
          interruptNumber + 0x20,
          interruptManager)
{
}

SyscallHandler::~SyscallHandler()
{
}

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    auto cpuState = (CPUState *)esp;

    switch (cpuState->eax)
    {
    case 4:
        Console::Write("%s", cpuState->ebx);
        break;
    case 60:
        kernal_running = false;
        break;
    default:
        Console::Write("Syscall '%d' not supported", cpuState->eax);
        break;
    }

    return esp;
}