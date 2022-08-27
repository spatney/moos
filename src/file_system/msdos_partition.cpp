#include <file_system/msdos_partition.h>
#include <common/console.h>
#include <file_system/fat.h>

using namespace moos::file_system;
using namespace moos::common;
using namespace moos::drivers;

int8_t *PartitionTable::ReadFileContents(
    AdvancedTechnologyAttachment *disk,
    const int8_t *name)
{
    MasterBootRecord mbr;
    disk->Read28(0, (uint8_t *)&mbr, sizeof(MasterBootRecord));

    if (mbr.magicNumber != 0xAA55)
    {
        Console::Write("illegal MBR\n");
        return nullptr;
    }

    for (int32_t i = 0; i < 4; i++)
    {
        auto partition = &mbr.primaryPartition[i];
        if (partition->partiotion_id == 0x00)
        {
            continue;
        }

        auto value = FATReader::ReadFileContents(name, disk, partition->start_lba);

        if (value != nullptr)
        {
            return value;
        }
    }

    return "";
}

void PartitionTable::ReadPartitions(AdvancedTechnologyAttachment *disk)
{
    MasterBootRecord mbr;
    disk->Read28(0, (uint8_t *)&mbr, sizeof(MasterBootRecord));

    /*Console::Write("MBR: ");

    for (int i = 446; i < 446 + 4 * 16 + 2; i++)
    {
        Console::Write("%x ", ((uint8_t *)&mbr)[i]);
    }

    Console::Write("\n");*/

    if (mbr.magicNumber != 0xAA55)
    {
        Console::Write("illegal MBR\n");
        return;
    }

    for (int32_t i = 0; i < 4; i++)
    {
        auto partition = &mbr.primaryPartition[i];
        if (partition->partiotion_id == 0x00)
        {
            continue;
        }

        Console::Write("Partition: %d", i);

        if (partition->bootable)
        {
            Console::Write(" bootable. Type ");
        }
        else
        {
            Console::Write(" not bootable. Type ");
        }

        Console::Write(
            "0x%x, start LBA: 0x%x\n",
            partition->partiotion_id,
            partition->start_lba);

        FATReader::ReadBiosBlock(disk, partition->start_lba);
    }
}