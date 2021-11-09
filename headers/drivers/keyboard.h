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
        enum Key
        {
            Unknown = -1,
            A = 0,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            Num0,
            Num1,
            Num2,
            Num3,
            Num4,
            Num5,
            Num6,
            Num7,
            Num8,
            Num9,
            Escape,
            LControl,
            LShift,
            LAlt,
            LSystem,
            RControl,
            RShift,
            RAlt,
            RSystem,
            Menu,
            LBracket,
            RBracket,
            Semicolon,
            Comma,
            Period,
            Quote,
            Slash,
            Backslash,
            Tilde,
            Equal,
            Hyphen,
            Space,
            Enter,
            Backspace,
            Tab,
            PageUp,
            PageDown,
            End,
            Home,
            Insert,
            Delete,
            Add,
            Subtract,
            Multiply,
            Divide,
            Left,
            Right,
            Up,
            Down,
            Numpad0,
            Numpad1,
            Numpad2,
            Numpad3,
            Numpad4,
            Numpad5,
            Numpad6,
            Numpad7,
            Numpad8,
            Numpad9,
            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            F13,
            F14,
            F15,
            Pause,
            KeyCount
        };

        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();

            virtual void OnKeyDown(common::int8_t);
            virtual void OnKeyUp(common::int8_t);

            // Update keyboard to pass enum in future
            // virtual void OnKeyDown(Keys key);
            // virtual void OnKeyUp(Keys key);
        };

        class KeyboardDriver : public hardware::InterruptHandler, public Driver
        {
        private:
            hardware::Port8Bit dataPort;
            hardware::Port8Bit commandPort;
            KeyboardEventHandler *eventHandler;

            bool isShiftDown;

        public:
            KeyboardDriver(
                hardware::InterruptManager *manager,
                KeyboardEventHandler *eventHandler);

            ~KeyboardDriver();
            virtual common::uint32_t HandleInterrupt(common::uint32_t esp);
            virtual void Activate();
        };
    }
}

#endif