#ifndef __MOOS__COMMON__STRINGS_H
#define __MOOS__COMMON__STRINGS_H

#include <common/types.h>

namespace moos
{
    namespace common
    {
        class StringUtil
        {
        public:
            static int32_t strlen(const moos::common::int8_t *s);
            static int32_t strcmp(const char *X, const char *Y);
        };
    }
}

#endif