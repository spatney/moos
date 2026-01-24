#ifndef __MOOS__GUI__Terminal_H
#define __MOOS__GUI__Terminal_H

#include <common/types.h>
#include <drivers/mouse.h>
#include <drivers/keyboard.h>
#include <common/linkedlist.h>
#include <core/sharedptr.h>

namespace moos
{
    namespace gui
    {
        class Editor;
        
        class Terminal : public drivers::MouseEventHandler, public drivers::KeyboardEventHandler
        {
        private:
            common::int32_t x;
            common::int32_t y;
            common::int32_t bufferCount;
            common::int32_t cursorPos;  // Position within buffer for inline editing

            common::uint8_t promptX;
            common::uint8_t promptY;
            common::int8_t *buffer;

            common::LinkedList *history;
            common::LinkedListNodeIterator historyIt;

            drivers::KeyboardDriver *keyboardDriver;
            Editor *activeEditor;

            bool isShiftDown;
            bool isCapsLockOn;
            bool isCtrlDown;

            void drawPrompt();
            void redrawInput();
            void insertCharAtCursor(common::int8_t c);
            void deleteCharAtCursor();
            void moveCursorLeft();
            void moveCursorRight();
            void moveCursorToStart();
            void moveCursorToEnd();
            void clearInput();
            common::int8_t *copyBuffer();
            common::size_t copyToBuffer(common::int8_t *src);
            common::LinkedList *tokenizeBuffer();
            common::uint8_t keyToChar(drivers::Key key);

        public:
            Terminal(drivers::KeyboardDriver *keyboardDriver);
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