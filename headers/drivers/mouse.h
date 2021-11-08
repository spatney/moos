#ifndef __MOOS__DRIVERS__MOUSE_H
#define __MOOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>
#include <drivers/driver.h>

namespace moos
{
    namespace drivers
    {
        class MouseEventHandler
        {
        public:
            MouseEventHandler();

            virtual void OnActivate();
            virtual void OnMouseDown(moos::common::uint8_t button);
            virtual void OnMouseUp(moos::common::uint8_t button);
            virtual void OnMouseMove(moos::common::int32_t x, moos::common::int32_t y);
        };

        class MouseDriver : public moos::hardware::InterruptHandler, public Driver
        {
            moos::hardware::Port8Bit dataPort;
            moos::hardware::Port8Bit commandPort;

            moos::common::uint8_t buffer[3];
            moos::common::uint8_t offset;
            moos::common::uint8_t buttons;

            MouseEventHandler *eventHandler;

        public:
            MouseDriver(moos::hardware::InterruptManager *manager, MouseEventHandler *eventHandler);
            ~MouseDriver();
            virtual moos::common::uint32_t HandleInterrupt(moos::common::uint32_t esp);
            virtual void Activate();
        };
    }
}

#endif