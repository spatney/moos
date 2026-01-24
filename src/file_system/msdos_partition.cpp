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
        Console::Printf("illegal MBR\n");
        return nullptr;
    }

    // Try partitions first
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

    // No partitions found - try superfloppy (FAT at sector 0)
    return FATReader::ReadFileContents(name, disk, 0);
}

void PartitionTable::ReadPartitions(AdvancedTechnologyAttachment *disk)
{
    MasterBootRecord mbr;
    disk->Read28(0, (uint8_t *)&mbr, sizeof(MasterBootRecord));

    /*Console::Printf("MBR: ");

    for (int i = 446; i < 446 + 4 * 16 + 2; i++)
    {
        Console::Printf("%x ", ((uint8_t *)&mbr)[i]);
    }

    Console::Printf("\n");*/

    if (mbr.magicNumber != 0xAA55)
    {
        Console::Printf("illegal MBR\n");
        return;
    }

    for (int32_t i = 0; i < 4; i++)
    {
        auto partition = &mbr.primaryPartition[i];
        if (partition->partiotion_id == 0x00)
        {
            continue;
        }

        Console::Printf("Partition: %d", i);

        if (partition->bootable)
        {
            Console::Printf(" bootable. Type ");
        }
        else
        {
            Console::Printf(" not bootable. Type ");
        }

        Console::Printf(
            "0x%x, start LBA: 0x%x\n",
            partition->partiotion_id,
            partition->start_lba);

        FATReader::ReadBiosBlock(disk, partition->start_lba);
    }
}

void PartitionTable::ListFiles(AdvancedTechnologyAttachment *disk)
{
    MasterBootRecord mbr;
    disk->Read28(0, (uint8_t *)&mbr, sizeof(MasterBootRecord));

    if (mbr.magicNumber != 0xAA55)
    {
        Console::Printf("illegal MBR\n");
        return;
    }

    // Try partitions first
    for (int32_t i = 0; i < 4; i++)
    {
        auto partition = &mbr.primaryPartition[i];
        if (partition->partiotion_id == 0x00)
        {
            continue;
        }

        FATReader::ListFiles(disk, partition->start_lba);
        return;
    }
    
    // No partitions - try superfloppy
    FATReader::ListFiles(disk, 0);
}

bool PartitionTable::WriteFileContents(
    AdvancedTechnologyAttachment *disk,
    const int8_t *name,
    const int8_t *data,
    uint32_t dataSize)
{
    if (disk == 0)
        return false;
        
    MasterBootRecord mbr;
    disk->Read28(0, (uint8_t *)&mbr, sizeof(MasterBootRecord));

    if (mbr.magicNumber != 0xAA55)
    {
        return false;
    }

    // Try partitions first
    for (int32_t i = 0; i < 4; i++)
    {
        auto partition = &mbr.primaryPartition[i];
        if (partition->partiotion_id == 0x00)
        {
            continue;
        }

        return FATReader::WriteFileContents(name, data, dataSize, disk, partition->start_lba);
    }

    // No partitions - try superfloppy (FAT at sector 0)
    return FATReader::WriteFileContents(name, data, dataSize, disk, 0);
}