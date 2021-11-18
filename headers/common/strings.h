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
            static int32_t strlen(const common::int8_t *s);
            static int32_t strcmp(const common::int8_t *X, const common::int8_t *Y);
        };
    }
}

#endif