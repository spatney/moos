#include <file_system/fat.h>
#include <common/console.h>

using namespace moos;
using namespace moos::common;
using namespace moos::file_system;
using namespace moos::drivers;

void FATReader::ReadBiosBlock(
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

    // Console::Write("Sectors per cluster: %d\n", bpb.sectorsPerCluster);

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

        uint32_t firstFileCluster = ((uint32_t)entry->firstClusterHi << 16) | ((uint32_t)entry->firstClusterLow);

        int32_t size = entry->size;
        int32_t nextFileCluster = firstFileCluster;

        uint8_t fatBuffer[513];
        auto content = new uint8_t[size + 1];
        content[size] = '\0';

        while (size > 0)
        {
            uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (nextFileCluster - 2);
            int32_t sectorOffset = 0;

            for (; size > 0; size -= 512)
            {
                if (sectorOffset >= bpb.sectorsPerCluster)
                {
                    break;
                }

                disk->Read28(fileSector + sectorOffset, &content[entry->size - size], size > 512 ? 512 : size);

                sectorOffset++;
            }

            uint32_t fatSectorForCurrentCluster = nextFileCluster / (512 / sizeof(uint32_t));

            disk->Read28(fatStart + fatSectorForCurrentCluster, fatBuffer, 512);

            uint32_t fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32_t));

            nextFileCluster = ((uint32_t *)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;
        }

        Console::Write("\n%s\n", content);
        delete content;
    }
}
