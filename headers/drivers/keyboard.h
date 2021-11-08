#ifndef __MOOS__DRIVERS__KEYBOARD_H
#define __MOOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>
#include <drivers/driver.h>

namespace moos
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();

            virtual void OnKeyDown(moos::common::int8_t);
            virtual void OnKeyUp(moos::common::int8_t);
        };

        class KeyboardDriver : public moos::hardware::InterruptHandler, public Driver
        {
            moos::hardware::Port8Bit dataPort;
            moos::hardware::Port8Bit commandPort;
            KeyboardEventHandler *eventHandler;

            bool isShiftDown;

        public:
            KeyboardDriver(moos::hardware::InterruptManager *manager, KeyboardEventHandler *eventHandler);
            ~KeyboardDriver();
            virtual moos::common::uint32_t HandleInterrupt(moos::common::uint32_t esp);
            virtual void Activate();
        };
    }
}

#endif