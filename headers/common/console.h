#ifndef __MOOS__COMMON__CONSOLE_H
#define __MOOS__COMMON__CONSOLE_H

#include <common/types.h>

namespace moos
{
    namespace common
    {
        class Console
        {
        public:
            static void Write(const int8_t *message, ...);
            static void Backspace();

        private:
            static void moveCursorBackByOne();
            static int8_t *itoa(int32_t val, const int32_t base);
        };
    }
}

#endif