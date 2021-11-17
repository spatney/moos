#ifndef __MOOS__COMMON__CONSOLE_H
#define __MOOS__COMMON__CONSOLE_H

#include <common/types.h>

namespace moos
{
    namespace common
    {

        struct Cursor
        {
            int8_t x = 0;
            int8_t y = 0;
        };

        class Console
        {
        public:
            inline static common::uint32_t ticks = 0;

            static void Clear();
            static void Write(const common::int8_t *message, ...);
            static void Backspace();
            static void Sleep(common::int32_t);
            static Cursor ReadCursor();

        private:
            static void enableCusor(
                common::uint8_t cursor_start,
                common::uint8_t cursor_end);

            static void updateCursor();
            static void disableCursor();
            static void moveCursorBackByOne();

            static common::int8_t *itoa(
                common::int32_t val,
                const common::int32_t base);
        };
    }
}

#endif