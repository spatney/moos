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

void PeripheralComponentInterconnectController::SelectDrivers(DriverManager *driverManager)
{
    for (uint8_t bus = 0; bus < 8; bus++)
    {
        for (uint8_t device = 0; device < 32; device++)
        {
            uint8_t numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;

            bool shouldPrintLine = false;

            for (uint8_t function = 0; function < numFunctions; function++)
            {
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);

                if (dev.vendor_id == 0x000 || dev.vendor_id == 0xFFFF)
                {
                    break;
                }

                Console::Write("PCI BUS %x, DEVICE %x, FUNCTION %x, VENDOR %x, DEVICE %x\n",
                               bus,
                               device,
                               function,
                               dev.vendor_id,
                               dev.device_id);
                shouldPrintLine = true;
            }

            if (shouldPrintLine)
            {
                for (uint32_t i = 0; i < 79; i++)
                {
                    Console::Write("-");
                }
                Console::Write("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(
    moos::common::uint16_t bus,
    moos::common::uint16_t device,
    moos::common::uint16_t function)
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