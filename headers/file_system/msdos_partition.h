#ifndef __MOOS__FILE_SYSTEM__MSDOS_PARTITION_H
#define __MOOS__FILE_SYSTEM__MSDOS_PARTITION_H

#include <common/types.h>
#include <drivers/ata.h>

namespace moos
{
    namespace file_system
    {
        struct PartitionTableEntry
        {
            common::uint8_t bootable;

            common::uint8_t start_head;
            common::uint8_t start_sector : 6;
            common::uint16_t start_cylinder : 10;

            common::uint8_t partiotion_id;

            common::uint8_t end_head;
            common::uint8_t end_sector : 6;
            common::uint16_t end_cylinder : 10;

            common::uint32_t start_lba;
            common::uint32_t length_lba;
        } __attribute__((packed));

        struct MasterBootRecord
        {
            common::uint8_t bootloader[440];
            common::uint32_t signature;
            common::uint16_t unused;

            PartitionTableEntry primaryPartition[4];
            common::uint16_t magicNumber;
        } __attribute__((packed));

        class PartitionTable
        {
        public:
            static common::int8_t *ReadFileContents(
                drivers::AdvancedTechnologyAttachment *disk,
                const common::int8_t *name);
            static void ReadPartitions(drivers::AdvancedTechnologyAttachment *disk);
        };
    }
}

#endif