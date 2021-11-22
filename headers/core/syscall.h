#ifndef __CORE__SYSCALL_H
#define __CORE__SYSCALL_H

#include <hardware/interrupts.h>
#include <common/types.h>
#include <core/multitasking.h>

namespace moos
{
    namespace core
    {
        class SyscallHandler : public hardware::InterruptHandler
        {
        private:
        public:
            SyscallHandler(
                hardware::InterruptManager *interruptManager,
                common::uint8_t interruptNumber);
            ~SyscallHandler();

            common::uint32_t HandleInterrupt(common::uint32_t esp);
        };

    }
}

#endif