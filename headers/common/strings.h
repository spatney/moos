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
            static common::int32_t strlen(const common::int8_t *s);
            static common::int32_t strcmp(const common::int8_t *X, const common::int8_t *Y);
            static common::int32_t atoi(const common::int8_t* S);
        };
    }
}

#endif