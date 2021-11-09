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
            moos::common::uint8_t *address;
            moos::common::uint32_t size;
            BaseAddressRegisterType type;
        };

        class PeripheralComponentInterconnectDeviceDescriptor
        {
        public:
            moos::common::uint32_t portBase;
            moos::common::uint32_t interrupt;

            moos::common::uint16_t bus;
            moos::common::uint16_t device;
            moos::common::uint16_t function;

            moos::common::uint16_t vendor_id;
            moos::common::uint16_t device_id;

            moos::common::uint8_t class_id;
            moos::common::uint8_t subclass_id;
            moos::common::uint8_t interface_id;

            moos::common::uint8_t revision;

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

            moos::common::uint32_t Read(
                moos::common::uint16_t bus,
                moos::common::uint16_t device,
                moos::common::uint16_t function,
                moos::common::uint32_t registerOffset);

            void Write(
                moos::common::uint16_t bus,
                moos::common::uint16_t device,
                moos::common::uint16_t function,
                moos::common::uint32_t registerOffset,
                moos::common::uint32_t value);

            bool DeviceHasFunctions(
                moos::common::uint16_t bus,
                moos::common::uint16_t device);

            void SelectDrivers(
                moos::drivers::DriverManager *driverManager, 
                moos::hardware::InterruptManager *InterruptManager);

            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(
                moos::common::uint16_t bus,
                moos::common::uint16_t device,
                moos::common::uint16_t function);

            BaseAddressRegister GetBaseAddressRegister(
                moos::common::uint16_t bus,
                moos::common::uint16_t device,
                moos::common::uint16_t function,
                moos::common::uint16_t bar);
        };
    }
}

#endif