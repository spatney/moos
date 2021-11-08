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
            static void Write(const moos::common::int8_t *message, ...);
            static void Backspace();
            static void Sleep(moos::common::int32_t);

        private:
            static void moveCursorBackByOne();
            static moos::common::int8_t *itoa(moos::common::int32_t val, const moos::common::int32_t base);
        };
    }
}

#endif