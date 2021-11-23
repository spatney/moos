#include <file_system/fat.h>
#include <common/console.h>

using namespace moos;
using namespace moos::common;
using namespace moos::file_system;
using namespace moos::drivers;

void moos::file_system::ReadBiosBlock(
    AdvancedTechnologyAttachment *disk,
    uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;
    disk->Read28(partitionOffset, (uint8_t *)&bpb, sizeof(BiosParameterBlock32));

    /*Console::Write("BiosParameterBlock32: ");

    for (int i = 0; i < sizeof(BiosParameterBlock32); i++)
    {
        Console::Write("%x ", ((uint8_t *)&bpb)[i]);
    }

    Console::Write("\n");*/

    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;
    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntryFat32 directoryEntries[16];
    disk->Read28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);

    // Console::Write("RootStart: 0x%x\n", rootStart);

    for (auto i = 0; i < 16; i++)
    {
        auto entry = &directoryEntries[i];
        if (entry->name[0] == 0x00)
        {
            break;
        }

        if ((entry->attributes & 0x0F) == 0x0F)
        {
            continue;
        }

        Console::Write("Name: %s\n", entry->name);

        if ((entry->attributes & 0x10) == 0x10) // directory
        {
            continue;
        }

        uint32_t fileCluster = ((uint32_t)entry->firstClusterHi << 16) | ((uint32_t)entry->firstClusterLow);
        uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (fileCluster - 2);

        uint8_t buffer[512];
        disk->Read28(fileSector, buffer, 512);
        buffer[entry->size] = '\0';
        Console::Write("File contents: %s\n", buffer);
    }
}
