#include <hardware/pci.h>
#include <common/console.h>

using namespace moos::hardware;
using namespace moos::common;
using namespace moos::drivers;

PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor()
{
}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor()
{
}

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
    : dataPort(0xCFC),
      commandPort(0xCF8)
{
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
}

uint32_t PeripheralComponentInterconnectController::Read(
    uint16_t bus,
    uint16_t device,
    uint16_t function,
    uint32_t registerOffset)
{
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registerOffset & 0xFC);

    commandPort.Write(id);

    uint32_t result = dataPort.Read();
    return result >> (8 * (registerOffset % 4));
}

void PeripheralComponentInterconnectController::Write(
    uint16_t bus,
    uint16_t device,
    uint16_t function,
    uint32_t registerOffset,
    uint32_t value)
{
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registerOffset & 0xFC);

    commandPort.Write(id);
    dataPort.Write(value);
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(
    uint16_t bus,
    uint16_t device)
{
    return Read(bus, device, 0, 0x0E) & (1 << 7);
}

void PeripheralComponentInterconnectController::SelectDrivers(DriverManager *driverManager, InterruptManager *interruptManager)
{
    for (uint8_t bus = 0; bus < 8; bus++)
    {
        for (uint8_t device = 0; device < 32; device++)
        {
            uint8_t numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;

            for (uint8_t function = 0; function < numFunctions; function++)
            {
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);

                if (dev.vendor_id == 0x000 || dev.vendor_id == 0xFFFF)
                {
                    continue;
                }

                for (uint8_t barNum = 0; barNum < 6; barNum++)
                {
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    if (bar.address && (bar.type == IO))
                    {
                        dev.portBase = (uint32_t)bar.address;
                    }

                    Driver *driver = GetDriver(dev, interruptManager);

                    if (driver != 0)
                    {
                        driverManager->AddDriver(driver);
                    }
                }

                Console::Write("BUS %x, DEVICE %x, FUNC %x, VENDOR %x, DEVICE %x, CLS %x - %x\n",
                               bus,
                               device,
                               function,
                               dev.vendor_id,
                               dev.device_id,
                               dev.class_id,
                               dev.subclass_id);
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(
    uint16_t bus,
    uint16_t device,
    uint16_t function)
{
    PeripheralComponentInterconnectDeviceDescriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0B);
    result.subclass_id = Read(bus, device, function, 0x0A);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3C);

    return result;
}

BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister(
    uint16_t bus,
    uint16_t device,
    uint16_t function,
    uint16_t bar)
{
    BaseAddressRegister result;

    uint32_t headerType = Read(bus, device, function, 0x0E) & 0x7F;
    uint8_t maxBars = 6 - (4 * headerType);

    if (bar >= maxBars)
    {
        return result;
    }

    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 0x1) ? IO : MemmoryMap;
    uint32_t temp;

    if (result.type == MemmoryMap)
    {
        // Console::Write("MEMORY MAPPED BAR NOT SUPPORTED");
        switch ((bar_value >> 1) & 0x3)
        {
        // TBD
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;

        default:
            break;
        }

        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
    }
    else
    {
        result.address = (uint8_t *)(bar_value & ~0x3);
        result.prefetchable = false;
    }

    return result;
}

Driver *PeripheralComponentInterconnectController::GetDriver(
    PeripheralComponentInterconnectDeviceDescriptor descriptor,
    InterruptManager *InterruptManager)
{
    switch (descriptor.vendor_id)
    {
    case 0x1022: // AMD
        switch (descriptor.device_id)
        {
        case 0x2000: //am79c973
            break;
        }
        break;
    case 0x8086: // Intel
        break;
    }

    switch (descriptor.class_id)
    {
    case 0x03: // graphics
        switch (descriptor.subclass_id)
        {
        case 0x00: // VGA
            // Console::Write("Loading VGA driver ... \n");
            break;
        }
    }
    return 0;
}