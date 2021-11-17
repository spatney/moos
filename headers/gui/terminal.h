#ifndef __MOOS__GUI__Terminal_H
#define __MOOS__GUI__Terminal_H

#include <common/types.h>
#include <drivers/mouse.h>
#include <drivers/keyboard.h>
#include <common/linkedlist.h>

namespace moos
{
    namespace gui
    {
        struct Token
        {
            common::uint8_t *str;
            common::uint8_t len;
        };

        class Terminal : public drivers::MouseEventHandler, public drivers::KeyboardEventHandler
        {
        private:
            common::int32_t x;
            common::int32_t y;
            common::uint8_t promptY;
            common::uint8_t *buffer;
            common::int32_t bufferCount;

            // all the keyboard state stuff should probably be in a state manager of sorts.
            // will attempt some other time.
            bool isShiftDown;
            bool isCapsLockOn;

            void drawPrompt();
            common::LinkedList *tokenizeBuffer();
            common::uint8_t keyToChar(drivers::Key key);

        public:
            Terminal();
            ~Terminal();

            void Reset();
            void OnKeyDown(drivers::Key key);
            void OnKeyUp(drivers::Key key);
            void OnMouseDown(common::uint8_t button);
            void OnMouseUp(common::uint8_t button);

            void OnMouseMove(
                common::int32_t xOffset,
                common::int32_t yOffset);

            void invertVideoMemoryAt(
                common::int8_t x,
                common::int8_t y);
        };
    }
}

#endif