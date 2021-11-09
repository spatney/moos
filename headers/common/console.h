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
            static void Clear();
            static void Write(const common::int8_t *message, ...);
            static void Backspace();
            static void Sleep(common::int32_t);

        private:
            static void moveCursorBackByOne();
            
            static common::int8_t *itoa(
                common::int32_t val, 
                const common::int32_t base);
        };
    }
}

#endif