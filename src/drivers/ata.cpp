#include <drivers/ata.h>
#include <common/console.h>

using namespace moos::drivers;
using namespace moos::common;

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(
    uint16_t portBase,
    bool master)
    : dataPort(portBase),
      errorPort(portBase + 1),
      sectorCountPort(portBase + 2),
      lbaLowPort(portBase + 3),
      lbaMidPort(portBase + 4),
      lbaHighPort(portBase + 5),
      devicePort(portBase + 6),
      commandPort(portBase + 7),
      controlPort(portBase + 0x206),
      master(master)
{
    bytesPerSector = 512;
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{
}

bool AdvancedTechnologyAttachment::Identify()
{
    devicePort.Write(master ? 0xA0 : 0xB0); // who to talk to.
    controlPort.Write(0);                   // clear HOB bit

    devicePort.Write(0xA0); // read status of master
    auto status = commandPort.Read();

    if (status == 0xFF)
    {
        Console::Write("No device found.\n");
        return false;
    }

    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHighPort.Write(0);
    commandPort.Write(0xEC); // identify

    status = commandPort.Read();

    if (status == 0)
    {
        Console::Write("No device found.\n");
        return false;
    }

    while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
    {
        status = commandPort.Read();
    }

    if (status & 0x01)
    {
        Console::Write("ATA Error.\n");
        return false;
    }

    auto buffer = new int8_t[512];

    for (int32_t i = 0; i < 512; i += 2)
    {
        auto data = dataPort.Read();
        buffer[i] = (data >> 8) & 0x00FF;
        buffer[i + 1] = data & 0x00FF;
    }

    int32_t offset = 46;
    int32_t len = 94 - offset;
    auto identifier = new int8_t[len + 1];

    for (int32_t i = 0; i < len; i++)
    {
        auto value = buffer[i + offset];
        identifier[i] = value == '\0' ? ' ' : value;
    }

    identifier[len] = '\0';

    Console::Write("%s\n", identifier);
    return true;
}

void AdvancedTechnologyAttachment::Read28(
    uint32_t sector,
    uint8_t *data,
    int32_t count)
{
    if (sector & 0xF0000000)
    {
        Console::Write("Cannot read from sector larger than 512\n");
        return;
    }

    if (count > bytesPerSector)
    {
        return;
    }

    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0X0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1); // single sector

    lbaLowPort.Write((sector & 0x000000FF));
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHighPort.Write((sector & 0x00FF0000) >> 8);

    commandPort.Write(0x20);

    auto status = commandPort.Read();

    while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
    {
        status = commandPort.Read();
    }

    if (status & 0x01)
    {
        Console::Write("ATA Error\n");
        return;
    }

    Console::Write("Reading from ATA: ");

    for (uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = dataPort.Read();

        int8_t *foo = "  \0";
        foo[1] = (wdata >> 8) & 0x00FF;
        foo[0] = wdata & 0x00FF;
        Console::Write("%s", foo);

        data[i] = wdata & 0x00FF;
        if (i + 1 < count)
            data[i + 1] = (wdata >> 8) & 0x00FF;
    }

    for (uint16_t i = count + (count % 2); i < bytesPerSector; i += 2)
    {
        dataPort.Read();
    }

    Console::Write("\n");
}

void AdvancedTechnologyAttachment::Write28(
    uint32_t sector,
    uint8_t *data,
    int32_t count)
{
    if (sector & 0xF0000000)
    {
        Console::Write("Cannot write to sector larger than 512\n");
        return;
    }

    if (count > bytesPerSector)
    {
        return;
    }

    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0X0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1); // single sector

    lbaLowPort.Write((sector & 0x000000FF));
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHighPort.Write((sector & 0x00FF0000) >> 8);

    commandPort.Write(0x30);

    Console::Write("Writing to ATA: ");

    for (uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = data[i];
        if (i + 1 < count)
            wdata |= ((uint16_t)data[i + 1]) << 8;

        int8_t *foo = "  \0";
        foo[1] = (wdata >> 8) & 0x00FF;
        foo[0] = wdata & 0x00FF;
        Console::Write("%s", foo);

        dataPort.Write(wdata);
    }

    for (uint16_t i = count + (count % 2); i < bytesPerSector; i += 2)
    {
        dataPort.Write(0);
    }

    Console::Write("\n");
}

void AdvancedTechnologyAttachment::Flush()
{
    devicePort.Write(master ? 0xE0 : 0xF0);
    commandPort.Write(0xE7);

    uint8_t status = commandPort.Read();
    if (status == 0x00)
        return;

    while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if (status & 0x01)
    {
        Console::Write("ERROR\n");
        return;
    }
}