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
            common::uint8_t interruptNumber;
            InterruptManager *interruptManager;

            InterruptHandler(
                common::uint8_t interruptNumber, 
                InterruptManager *interruptManager);
            ~InterruptHandler();

        public:
            virtual common::uint32_t HandleInterrupt(common::uint32_t esp);
        };

        class InterruptManager
        {
            friend class InterruptHandler;

        protected:
            static InterruptManager *ActivateInterruptManager;
            InterruptHandler *handlers[256];

            struct GateDescriptor
            {
                common::uint16_t handlerAddressLowBits;
                common::uint16_t gdt_codeSegmentSelector;
                common::uint8_t reserved;
                common::uint8_t access;
                common::uint16_t handlerAddressHighBits;
            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];

            struct interruptDescriptorTablePointer
            {
                common::uint16_t size;
                common::uint32_t base;
            } __attribute__((packed));

            static void SetInterruptDescriptorTableEntry(
                common::uint8_t interruptNumber,
                common::uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                common::uint8_t DescriptorPrivilageLevel,
                common::uint8_t DescriptorType);

            Port8BitSlow picMasterCommand;
            Port8BitSlow picMasterData;
            Port8BitSlow picSlaveCommand;
            Port8BitSlow picSlaveData;

        public:
            InterruptManager(GlobalDescriptorTable *gdt);
            ~InterruptManager();

            void Activate();
            void Deactivate();

            static common::uint32_t HandleInterrupt(common::uint8_t interruptNumber, common::uint32_t esp);
            common::uint32_t DoHandleInterrupt(common::uint8_t interruptNumber, common::uint32_t esp);

            static void IgnoreInterruptRequest();
            static void HandleInterruptRequest0x0C(); //mouse
            static void HandleInterruptRequest0x01(); //keyboard
            static void HandleInterruptRequest0x00(); //timer
        };
    }
}

#endif
