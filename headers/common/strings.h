#ifndef __MOOS__COMMON__STRINGS_H
#define __MOOS__COMMON__STRINGS_H

#include <common/types.h>

namespace moos {
    namespace common {
        class StringUtil
        {
            static int32_t strlen(const moos::common::int8_t *s);
        };
    }
}

#endif