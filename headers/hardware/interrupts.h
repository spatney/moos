#ifndef __MOOS__HARDWARE__INTERRUPTS_H
#define __MOOS__HARDWARE__INTERRUPTS_H

#include <common/types.h>
#include <hardware/port.h>
#include <gdt.h>

namespace moos
{
    namespace hardware
    {
        class InterruptManager;

        class InterruptHandler
        {
        protected:
            moos::common::uint8_t interruptNumber;
            InterruptManager *interruptManager;

            InterruptHandler(moos::common::uint8_t interruptNumber, InterruptManager *interruptManager);
            ~InterruptHandler();

        public:
            virtual moos::common::uint32_t HandleInterrupt(moos::common::uint32_t esp);
        };

        class InterruptManager
        {
            friend class InterruptHandler;

        protected:
            static InterruptManager *ActivateInterruptManager;
            InterruptHandler *handlers[256];

            struct GateDescriptor
            {
                moos::common::uint16_t handlerAddressLowBits;
                moos::common::uint16_t gdt_codeSegmentSelector;
                moos::common::uint8_t reserved;
                moos::common::uint8_t access;
                moos::common::uint16_t handlerAddressHighBits;
            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];

            struct interruptDescriptorTablePointer
            {
                moos::common::uint16_t size;
                moos::common::uint32_t base;
            } __attribute__((packed));

            static void SetInterruptDescriptorTableEntry(
                moos::common::uint8_t interruptNumber,
                moos::common::uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                moos::common::uint8_t DescriptorPrivilageLevel,
                moos::common::uint8_t DescriptorType);

            Port8BitSlow picMasterCommand;
            Port8BitSlow picMasterData;
            Port8BitSlow picSlaveCommand;
            Port8BitSlow picSlaveData;

        public:
            InterruptManager(GlobalDescriptorTable *gdt);
            ~InterruptManager();

            void Activate();
            void Deactivate();

            static moos::common::uint32_t HandleInterrupt(moos::common::uint8_t interruptNumber, moos::common::uint32_t esp);
            moos::common::uint32_t DoHandleInterrupt(moos::common::uint8_t interruptNumber, moos::common::uint32_t esp);

            static void IgnoreInterruptRequest();
            static void HandleInterruptRequest0x0C(); //mouse
            static void HandleInterruptRequest0x01(); //keyboard
            static void HandleInterruptRequest0x00(); //timer
        };
    }
}

#endif
