#ifndef __MOOS__HARDWARE__PCI_H
#define __MOOS__HARDWARE__PCI_H

#include <common/types.h>
#include <hardware/port.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>

namespace moos
{
    namespace hardware
    {
        enum BaseAddressRegisterType
        {
            IO = 0,
            MemmoryMap = 1
        };

        class BaseAddressRegister
        {
        public:
            bool prefetchable;
            common::uint8_t *address;
            common::uint32_t size;
            BaseAddressRegisterType type;
        };

        class PeripheralComponentInterconnectDeviceDescriptor
        {
        public:
            common::uint32_t portBase;
            common::uint32_t interrupt;

            common::uint16_t bus;
            common::uint16_t device;
            common::uint16_t function;

            common::uint16_t vendor_id;
            common::uint16_t device_id;

            common::uint8_t class_id;
            common::uint8_t subclass_id;
            common::uint8_t interface_id;

            common::uint8_t revision;

            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();
        };

        class PeripheralComponentInterconnectController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;

        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();

            common::uint32_t Read(
                common::uint16_t bus,
                common::uint16_t device,
                common::uint16_t function,
                common::uint32_t registerOffset);

            void Write(
                common::uint16_t bus,
                common::uint16_t device,
                common::uint16_t function,
                common::uint32_t registerOffset,
                common::uint32_t value);

            bool DeviceHasFunctions(
                common::uint16_t bus,
                common::uint16_t device);

            void SelectDrivers(
                drivers::DriverManager *driverManager, 
                hardware::InterruptManager *InterruptManager);

            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(
                common::uint16_t bus,
                common::uint16_t device,
                common::uint16_t function);

            BaseAddressRegister GetBaseAddressRegister(
                common::uint16_t bus,
                common::uint16_t device,
                common::uint16_t function,
                common::uint16_t bar);

            drivers::Driver *GetDriver(
                PeripheralComponentInterconnectDeviceDescriptor descriptor,
                hardware::InterruptManager *InterruptManager);
        };
    }
}

#endif