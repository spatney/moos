#include <file_system/fat.h>
#include <common/console.h>
#include <common/strings.h>

using namespace moos;
using namespace moos::common;
using namespace moos::file_system;
using namespace moos::drivers;

int8_t *FATReader::ReadFileContents(
    const int8_t *name,
    drivers::AdvancedTechnologyAttachment *disk,
    uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;
    disk->Read28(partitionOffset, (uint8_t *)&bpb, sizeof(BiosParameterBlock32));
    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;
    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntryFat32 directoryEntries[16];
    disk->Read28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);

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

        if ((entry->attributes & 0x10) == 0x10) // directory
        {
            continue;
        }

        // Yup, super hacky indeed :)
        // but again, this isn't a serious OS
        // Build filename from entry for comparison
        int8_t entryName[13];
        int j = 0;
        for (int k = 0; k < 8 && entry->name[k] != ' '; k++)
        {
            entryName[j++] = entry->name[k];
        }
        if (entry->ext[0] != ' ')
        {
            entryName[j++] = '.';
            for (int k = 0; k < 3 && entry->ext[k] != ' '; k++)
            {
                entryName[j++] = entry->ext[k];
            }
        }
        entryName[j] = '\0';

        // Case-insensitive comparison
        auto match = true;
        for (int k = 0; entryName[k] != '\0' || name[k] != '\0'; k++)
        {
            int8_t c1 = entryName[k];
            int8_t c2 = name[k];
            // Convert to uppercase for comparison
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2)
            {
                match = false;
                break;
            }
        }

        if (!match)
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
        delete content;
        return (int8_t *)content;
    }

    return nullptr;
}

void FATReader::ListFiles(
    AdvancedTechnologyAttachment *disk,
    uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;
    disk->Read28(partitionOffset, (uint8_t *)&bpb, sizeof(BiosParameterBlock32));
    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;
    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntryFat32 directoryEntries[16];
    disk->Read28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);

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

        // Print filename (8 chars) and extension (3 chars)
        int8_t filename[13];
        int j = 0;
        for (int k = 0; k < 8 && entry->name[k] != ' '; k++)
        {
            filename[j++] = entry->name[k];
        }
        if (entry->ext[0] != ' ')
        {
            filename[j++] = '.';
            for (int k = 0; k < 3 && entry->ext[k] != ' '; k++)
            {
                filename[j++] = entry->ext[k];
            }
        }
        filename[j] = '\0';

        if ((entry->attributes & 0x10) == 0x10)
        {
            // Directory
            auto color = Console::SetColor(0x09);
            Console::Printf("%s/\n", filename);
            Console::SetColor(color);
        }
        else
        {
            Console::Printf("%s  %d bytes\n", filename, entry->size);
        }
    }
}

void FATReader::ReadBiosBlock(
    AdvancedTechnologyAttachment *disk,
    uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;
    disk->Read28(partitionOffset, (uint8_t *)&bpb, sizeof(BiosParameterBlock32));

    /*Console::Printf("BiosParameterBlock32: ");

    for (int i = 0; i < sizeof(BiosParameterBlock32); i++)
    {
        Console::Printf("%x ", ((uint8_t *)&bpb)[i]);
    }

    Console::Printf("\n");*/

    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;
    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntryFat32 directoryEntries[16];
    disk->Read28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);

    // Console::Printf("Sectors per cluster: %d\n", bpb.sectorsPerCluster);

    // Console::Printf("RootStart: 0x%x\n", rootStart);

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

        Console::Printf("Name: %s\n", entry->name);

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

        Console::Printf("\n%s\n", content);
        delete content;
    }
}

uint32_t FATReader::GetPartitionOffset(AdvancedTechnologyAttachment *disk)
{
    if (disk == 0)
        return 0;
        
    // Read first sector
    uint8_t sector[512];
    disk->Read28(0, sector, 512);
    
    // Check for MBR magic number at offset 510-511
    if (sector[510] == 0x55 && sector[511] == 0xAA)
    {
        // Could be MBR or could be superfloppy FAT boot sector
        // Check if this looks like a FAT boot sector (has "FAT" at offset 54 or 82)
        bool isFatBoot = (sector[54] == 'F' && sector[55] == 'A' && sector[56] == 'T') ||
                         (sector[82] == 'F' && sector[83] == 'A' && sector[84] == 'T');
        
        if (isFatBoot)
        {
            // This is a superfloppy - FAT filesystem starts at sector 0
            return 0;
        }
        
        // This is an MBR - look for first valid partition
        // Partition table starts at offset 446
        for (int i = 0; i < 4; i++)
        {
            int offset = 446 + i * 16;
            uint8_t partType = sector[offset + 4];
            if (partType != 0x00)
            {
                // Found a partition - get its start LBA
                uint32_t startLba = sector[offset + 8] | 
                                   (sector[offset + 9] << 8) |
                                   (sector[offset + 10] << 16) |
                                   (sector[offset + 11] << 24);
                return startLba;
            }
        }
    }
    
    // Try treating it as a superfloppy anyway (offset 0)
    return 0;
}

bool FATReader::WriteFileContents(
    const int8_t *name,
    const int8_t *data,
    uint32_t dataSize,
    AdvancedTechnologyAttachment *disk,
    uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;
    disk->Read28(partitionOffset, (uint8_t *)&bpb, sizeof(BiosParameterBlock32));
    
    // Validate BPB - check for FAT signature
    if (bpb.bytesPerSector != 512)
    {
        return false;
    }
    
    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;
    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntryFat32 directoryEntries[16];
    disk->Read28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);

    // First pass: look for existing file to update
    for (auto i = 0; i < 16; i++)
    {
        auto entry = &directoryEntries[i];
        if (entry->name[0] == 0x00)
        {
            break;  // No more entries
        }

        if ((entry->attributes & 0x0F) == 0x0F)
        {
            continue;  // Long filename entry
        }

        if ((entry->attributes & 0x10) == 0x10)
        {
            continue;  // Directory
        }

        // Build filename from entry for comparison
        int8_t entryName[13];
        int j = 0;
        for (int k = 0; k < 8 && entry->name[k] != ' '; k++)
        {
            entryName[j++] = entry->name[k];
        }
        if (entry->ext[0] != ' ')
        {
            entryName[j++] = '.';
            for (int k = 0; k < 3 && entry->ext[k] != ' '; k++)
            {
                entryName[j++] = entry->ext[k];
            }
        }
        entryName[j] = '\0';

        // Case-insensitive comparison
        auto match = true;
        for (int k = 0; entryName[k] != '\0' || name[k] != '\0'; k++)
        {
            int8_t c1 = entryName[k];
            int8_t c2 = name[k];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2)
            {
                match = false;
                break;
            }
        }

        if (!match)
        {
            continue;
        }

        // Found the file - write data to its clusters
        uint32_t firstFileCluster = ((uint32_t)entry->firstClusterHi << 16) | ((uint32_t)entry->firstClusterLow);
        uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (firstFileCluster - 2);
        
        // Write data sector by sector (max 512 bytes per sector)
        uint32_t bytesWritten = 0;
        uint32_t sectorOffset = 0;
        
        while (bytesWritten < dataSize && sectorOffset < bpb.sectorsPerCluster)
        {
            uint32_t bytesToWrite = dataSize - bytesWritten;
            if (bytesToWrite > 512)
                bytesToWrite = 512;
            
            uint8_t sectorBuffer[512];
            for (uint32_t b = 0; b < 512; b++)
            {
                if (b < bytesToWrite)
                    sectorBuffer[b] = data[bytesWritten + b];
                else
                    sectorBuffer[b] = 0;
            }
            
            disk->Write28(fileSector + sectorOffset, sectorBuffer, 512);
            
            bytesWritten += bytesToWrite;
            sectorOffset++;
        }
        
        // Update file size in directory entry
        entry->size = dataSize;
        disk->Write28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);
        
        disk->Flush();
        
        return true;
    }

    // File not found - create new file
    // Find a free directory entry
    int freeEntry = -1;
    for (int i = 0; i < 16; i++)
    {
        if (directoryEntries[i].name[0] == 0x00 || (uint8_t)directoryEntries[i].name[0] == 0xE5)
        {
            freeEntry = i;
            break;
        }
    }
    
    if (freeEntry < 0)
        return false; // No free directory entry
    
    uint32_t freeCluster = 0;
    
    // Only allocate cluster if file has content
    if (dataSize > 0)
    {
        // Find a free cluster in FAT
        uint8_t fatBuffer[512];
        
        for (uint32_t fatSector = 0; fatSector < fatSize && freeCluster == 0; fatSector++)
        {
            disk->Read28(fatStart + fatSector, fatBuffer, 512);
            for (uint32_t j = 0; j < 128 && freeCluster == 0; j++) // 128 entries per sector (4 bytes each)
            {
                uint32_t cluster = fatSector * 128 + j;
                if (cluster < 2) continue; // Clusters 0 and 1 are reserved
                
                uint32_t fatEntry = *((uint32_t*)&fatBuffer[j * 4]) & 0x0FFFFFFF;
                if (fatEntry == 0)
                {
                    freeCluster = cluster;
                    // Mark cluster as end-of-chain
                    *((uint32_t*)&fatBuffer[j * 4]) = 0x0FFFFFFF;
                    disk->Write28(fatStart + fatSector, fatBuffer, 512);
                    // Update second FAT copy
                    disk->Write28(fatStart + fatSize + fatSector, fatBuffer, 512);
                }
            }
        }
        
        if (freeCluster == 0)
            return false; // No free cluster
    }
    
    // Parse filename into 8.3 format
    auto entry = &directoryEntries[freeEntry];
    for (int i = 0; i < 8; i++) entry->name[i] = ' ';
    for (int i = 0; i < 3; i++) entry->ext[i] = ' ';
    
    int namePos = 0;
    int extPos = 0;
    bool inExt = false;
    
    for (int i = 0; name[i] != '\0'; i++)
    {
        int8_t c = name[i];
        if (c >= 'a' && c <= 'z') c -= 32; // Uppercase
        
        if (c == '.')
        {
            inExt = true;
            continue;
        }
        
        if (!inExt && namePos < 8)
            entry->name[namePos++] = c;
        else if (inExt && extPos < 3)
            entry->ext[extPos++] = c;
    }
    
    entry->attributes = 0x20; // Archive
    entry->reserved = 0;
    entry->cTimeTenth = 0;
    entry->cTime = 0;
    entry->cDate = 0;
    entry->aTime = 0;
    entry->firstClusterHi = (freeCluster >> 16) & 0xFFFF;
    entry->wTime = 0;
    entry->wDate = 0;
    entry->firstClusterLow = freeCluster & 0xFFFF;
    entry->size = dataSize;
    
    // Write file data to cluster (only if there's content)
    if (dataSize > 0 && freeCluster > 0)
    {
        uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (freeCluster - 2);
        uint32_t bytesWritten = 0;
        uint32_t sectorOffset = 0;
        
        while (bytesWritten < dataSize && sectorOffset < bpb.sectorsPerCluster)
        {
            uint32_t bytesToWrite = dataSize - bytesWritten;
            if (bytesToWrite > 512)
                bytesToWrite = 512;
            
            uint8_t sectorBuffer[512];
            for (uint32_t b = 0; b < 512; b++)
            {
                if (b < bytesToWrite)
                    sectorBuffer[b] = data[bytesWritten + b];
                else
                    sectorBuffer[b] = 0;
            }
            
            disk->Write28(fileSector + sectorOffset, sectorBuffer, 512);
            
            bytesWritten += bytesToWrite;
            sectorOffset++;
        }
    }
    
    // Write updated directory
    disk->Write28(rootStart, (uint8_t *)&directoryEntries[0], sizeof(DirectoryEntryFat32) * 16);
    disk->Flush();
    
    return true;
}
