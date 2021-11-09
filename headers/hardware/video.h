#ifndef __MOOS__HARDWARE__VIDEO_H
#define __MOOS__HARDWARE__VIDEO_H

#include <common/types.h>

namespace moos
{
    namespace hardware
    {
        static common::uint16_t *VideoMemory = (common::uint16_t *)0xb8000;
    }
}

#endif