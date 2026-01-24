#ifndef __MOOS__GUI__EDITOR_H
#define __MOOS__GUI__EDITOR_H

#include <common/types.h>
#include <drivers/keyboard.h>

namespace moos
{
    namespace gui
    {
        class Editor : public drivers::KeyboardEventHandler
        {
        private:
            common::int8_t *buffer;
            common::int8_t *filename;
            common::size_t bufferSize;
            common::size_t bufferCapacity;
            
            common::int32_t cursorX;
            common::int32_t cursorY;
            common::int32_t scrollOffset;
            
            drivers::KeyboardDriver *keyboardDriver;
            drivers::KeyboardEventHandler *previousHandler;
            
            bool isShiftDown;
            bool modified;
            bool running;
            
            static const common::int32_t SCREEN_WIDTH = 80;
            static const common::int32_t SCREEN_HEIGHT = 25;
            static const common::int32_t EDIT_HEIGHT = 22;
            
            void draw();
            void moveCursor(common::int32_t dx, common::int32_t dy);
            void insertChar(common::int8_t c);
            void deleteChar();
            void newLine();
            common::size_t getLineStart(common::int32_t line);
            common::size_t getLineLength(common::int32_t line);
            common::int32_t getLineCount();
            common::size_t getCursorPosition();
            common::uint8_t keyToChar(drivers::Key key);
            void save();
            void exit();
            
        public:
            Editor(drivers::KeyboardDriver *keyboardDriver,
                   drivers::KeyboardEventHandler *previousHandler);
            ~Editor();
            
            void Open(const common::int8_t *filename, common::int8_t *contents);
            void OnKeyDown(drivers::Key key);
            void OnKeyUp(drivers::Key key);
            bool IsRunning() { return running; }
        };
    }
}

#endif
