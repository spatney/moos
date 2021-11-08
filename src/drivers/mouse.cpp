#include <drivers/mouse.h>
#include <hardware/video.h>

using namespace moos::drivers;
using namespace moos::hardware;
using namespace moos::common;

MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::OnActivate() {}
void MouseEventHandler::OnMouseDown(uint8_t button) {}
void MouseEventHandler::OnMouseUp(uint8_t button) {}
void MouseEventHandler::OnMouseMove(int32_t x, int32_t y) {}

MouseDriver::MouseDriver(InterruptManager *manager, MouseEventHandler *eventHandler)
    : InterruptHandler(0x2C, manager),
      dataPort(0x60),
      commandPort(0x64)
{
    this->eventHandler = eventHandler;
}

MouseDriver::~MouseDriver()
{
}

void MouseDriver::Activate()
{
    offset = 0;
    buttons = 0;

    commandPort.Write(0xA8); // ask mouse to start sending interrupts
    commandPort.Write(0x20); // get current state
    uint8_t status = dataPort.Read() | 2;
    commandPort.Write(0x60); // set state
    dataPort.Write(status);

    commandPort.Write(0xD4);
    dataPort.Write(0xF4); // activate mouse;
    dataPort.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandPort.Read();

    if ((!(status & 0x20)) || eventHandler == 0)
    {
        return esp;
    }

    buffer[offset] = dataPort.Read();
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
        {
            eventHandler->OnMouseMove(buffer[1], -buffer[2]);
        }

        for (uint8_t i = 0; i < 3; i++)
        {
            if ((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                if (buttons & (0x1 << 1))
                {
                    eventHandler->OnMouseUp(i + 1);
                }
                else
                {
                    eventHandler->OnMouseDown(i + 1);
                }
            }
        }
        buttons = buffer[0];
    }

    return esp;
}