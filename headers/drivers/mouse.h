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
            virtual void OnMouseDown(common::uint8_t button);
            virtual void OnMouseUp(common::uint8_t button);

            virtual void OnMouseMove(
                common::int32_t x,
                common::int32_t y);
        };

        class MouseDriver : public hardware::InterruptHandler, public Driver
        {
        private:
            hardware::Port8Bit dataPort;
            hardware::Port8Bit commandPort;

            common::uint8_t buffer[3];
            common::uint8_t offset;
            common::uint8_t buttons;

            MouseEventHandler *eventHandler;

        public:
            MouseDriver(
                hardware::InterruptManager *manager,
                MouseEventHandler *eventHandler);

            ~MouseDriver();
            virtual common::uint32_t HandleInterrupt(common::uint32_t esp);
            virtual void Activate();
        };
    }
}

#endif