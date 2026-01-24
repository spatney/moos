#include <drivers/disk.h>
#include <core/memory.h>
#include <common/console.h>

using namespace moos::drivers;
using namespace moos::common;

AdvancedTechnologyAttachment *DiskManager::primaryDisk = 0;

void DiskManager::Initialize()
{
    Console::Printf("Initializing disk subsystem...\n");
    
    // Try master on primary ATA controller
    auto ata0m = new AdvancedTechnologyAttachment(0x1F0, true);
    if (ata0m->Identify())
    {
        Console::Printf("Found disk: ATA primary master\n");
        primaryDisk = ata0m;
        return;
    }
    
    // Try slave on primary ATA controller  
    auto ata0s = new AdvancedTechnologyAttachment(0x1F0, false);
    if (ata0s->Identify())
    {
        Console::Printf("Found disk: ATA primary slave\n");
        primaryDisk = ata0s;
        return;
    }
    
    // No disk found
    Console::Printf("Warning: No disk found\n");
    primaryDisk = 0;
}
