#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;

class InterruptHandler {
protected:
    uint8_t interruptNumber;
    InterruptManager* interruptManager;

    InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
    ~InterruptHandler();

public:
    virtual uint32_t  HandleInterrupt(uint32_t esp);
};

class InterruptManager
{
    friend class InterruptHandler;
protected:

    static InterruptManager* ActivateInterruptManager;
    InterruptHandler* handlers[256];
    
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    static GateDescriptor interruptDescriptorTable[256];

    struct interruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));
    
    
    static void SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void(*handler)(),
        uint8_t DescriptorPrivilageLevel,
        uint8_t DescriptorType
    );

    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;
    
public:

    InterruptManager(GlobalDescriptorTable* gdt);
    ~InterruptManager();

    void Activate();
    void Deactivate();

    static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);
    uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);

    static void ignoreInterruptRequest();
    static void handleInterruptRequest0x0C(); //mouse
    static void handleInterruptRequest0x01(); //keyboard
    static void handleInterruptRequest0x00(); //timer
};

#endif
