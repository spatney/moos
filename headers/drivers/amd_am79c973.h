#ifndef __MOOS__DRIVERS__AMD_AM79C973_H
#define __MOOS__DRIVERS__AMD_AM79C973_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <hardware/port.h>

namespace moos
{
    namespace drivers
    {

        class amd_am79c973 : public Driver, public hardware::InterruptHandler
        {
            struct InitializationBlock
            {
                common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;
                common::uint64_t physicalAddress : 48;
                common::uint16_t reserved3;
                common::uint64_t logicalAddress;
                common::uint32_t recvBufferDescrAddress;
                common::uint32_t sendBufferDescrAddress;
            } __attribute__((packed));

            struct BufferDescriptor
            {
                common::uint32_t address;
                common::uint32_t flags;
                common::uint32_t flags2;
                common::uint32_t avail;
            } __attribute__((packed));

        private:
            hardware::Port16Bit MACAddress0Port;
            hardware::Port16Bit MACAddress2Port;
            hardware::Port16Bit MACAddress4Port;
            hardware::Port16Bit registerDataPort;
            hardware::Port16Bit registerAddressPort;
            hardware::Port16Bit resetPort;
            hardware::Port16Bit busControlRegisterDataPort;

            InitializationBlock initBlock;

            BufferDescriptor *sendBufferDescr;
            common::uint8_t sendbufferDescMemory[2 * 1024 + 15];
            common::uint8_t sendbuffers[2 * 1024 + 15][8];
            common::uint8_t currentSendBuffer;

            BufferDescriptor *recvBufferDescr;
            common::uint8_t recvbufferDescMemory[2 * 1024 + 15];
            common::uint8_t recvbuffers[2 * 1024 + 15][8];
            common::uint8_t currentRecvBuffer;

        public:
            amd_am79c973(
                hardware::PeripheralComponentInterconnectDeviceDescriptor *descriptor,
                hardware::InterruptManager *interrupts);
            ~amd_am79c973();

            void Activate();
            common::int32_t Reset();
            common::uint32_t HandleInterrupt(common::uint32_t esp);

            void Send(
                common::uint8_t *buffer,
                common::int32_t size);
            void Recieve();
        };
    }
}

#endif