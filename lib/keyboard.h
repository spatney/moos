#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();

    virtual void OnKeyDown(int8_t);
    virtual void OnKeyUp(int8_t);
};

class KeyboardDriver : public InterruptHandler, public Driver
{
    Port8Bit dataPort;
    Port8Bit commandPort;
    KeyboardEventHandler* eventHandler;

    bool isShiftDown;

public:
    KeyboardDriver(InterruptManager *manager, KeyboardEventHandler *eventHandler);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
};

#endif