#include "mouse.h"
#include "stdlib.h"
#include "hardware.h"

void MouseDriver::invertVideoMemoryAt(int8_t x, int8_t y) {
    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 
    | (VideoMemory[80 * y + x] & 0xF000) >> 4 
    | (VideoMemory[80 * y + x] & 0x00FF);
}

MouseDriver::MouseDriver(InterruptManager *manager)
    : InterruptHandler(0x2C, manager),
      dataPort(0x60),
      commandPort(0x64)
{
    offset = 0;
    buttons = 0;

    invertVideoMemoryAt(40, 12);

    commandPort.Write(0xA8); // ask mouse to start sending interrupts
    commandPort.Write(0x20); // get current state
    uint8_t status = dataPort.Read() | 2;
    commandPort.Write(0x60); // set state
    dataPort.Write(status);

    commandPort.Write(0xD4);
    dataPort.Write(0xF4); // activate mouse;
    dataPort.Read();
}

MouseDriver::~MouseDriver()
{
}
uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandPort.Read();

    if (!(status & 0x20))
    {
        return esp;
    }

    static int8_t x = 40, y = 12;

    buffer[offset] = dataPort.Read();
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        invertVideoMemoryAt(x,y);

        x += buffer[1];
        if (x >= 80)
            x = 79;
        if (x < 0)
            x = 0;
        y -= buffer[2];
        if (y >= 25)
            y = 24;
        if (y < 0)
            y = 0;

        invertVideoMemoryAt(x,y);

        for (uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))) {
                invertVideoMemoryAt(x,y);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}