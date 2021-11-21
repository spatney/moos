#ifndef __MOOS__DRIVERS__ATA_H
#define __MOOS__DRIVERS__ATA_H

#include <common/types.h>
#include <hardware/port.h>

namespace moos
{
    namespace drivers
    {
        class AdvancedTechnologyAttachment
        {
        private:
        protected:
            hardware::Port16Bit dataPort;
            hardware::Port8Bit errorPort;
            hardware::Port8Bit sectorCountPort;
            hardware::Port8Bit lbaLowPort;
            hardware::Port8Bit lbaMidPort;
            hardware::Port8Bit lbaHighPort;
            hardware::Port8Bit devicePort;
            hardware::Port8Bit commandPort;
            hardware::Port8Bit controlPort;

            bool master;
            common::uint16_t bytesPerSector;

        public:
            AdvancedTechnologyAttachment(
                common::uint16_t portBase,
                bool master);
            ~AdvancedTechnologyAttachment();

            void Identify();
            void Read28(common::uint32_t sector,
                common::uint8_t *data,
                common::int32_t count);
            void Write28(
                common::uint32_t sector,
                common::uint8_t *data,
                common::int32_t count);
            void Flush();
        };
    }
}

#endif