#ifndef __MOOS__DRIVERS__DISK_H
#define __MOOS__DRIVERS__DISK_H

#include <drivers/ata.h>

namespace moos
{
    namespace drivers
    {
        // Global disk manager - provides access to the primary disk
        class DiskManager
        {
        public:
            static AdvancedTechnologyAttachment *primaryDisk;
            
            static void Initialize();
            static AdvancedTechnologyAttachment *GetDisk() { return primaryDisk; }
        };
    }
}

#endif
