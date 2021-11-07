#include "keyboard.h"
#include "stdlib.h"

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(int8_t)
{
}
void KeyboardEventHandler::OnKeyUp(int8_t)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager *manager, KeyboardEventHandler *eventHandler)
    : InterruptHandler(0x21, manager),
      dataPort(0x60),
      commandPort(0x64)
{
    this->eventHandler = eventHandler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void KeyboardDriver::Activate()
{
    while (commandPort.Read() & 0x1)
    {
        dataPort.Read();
    }
    commandPort.Write(0xAE); // ask keyboard to start sending interrupts
    commandPort.Write(0x20); // get current state
    uint8_t status = (dataPort.Read() | 1) & ~0x10;
    commandPort.Write(0x60); // set state
    dataPort.Write(status);

    dataPort.Write(0xF4); // activate keyboard;
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{

    uint8_t key = dataPort.Read();

    if(eventHandler == 0) {
        return esp;
    }

    if (key >= 0x80 || key == 0xFA || key == 0x45)
    {
        // ignore 'key down'
        return esp;
    }

    switch (key)
    {
    case 0x02:
        eventHandler->OnKeyDown('1');
        break;
    case 0x03:
        eventHandler->OnKeyDown('2');
        break;
    case 0x04:
        eventHandler->OnKeyDown('3');
        break;
    case 0x05:
        eventHandler->OnKeyDown('4');
        break;
    case 0x06:
        eventHandler->OnKeyDown('5');
        break;
    case 0x07:
        eventHandler->OnKeyDown('6');
        break;
    case 0x08:
        eventHandler->OnKeyDown('7');
        break;
    case 0x09:
        eventHandler->OnKeyDown('8');
        break;
    case 0x0A:
        eventHandler->OnKeyDown('9');
        break;
    case 0x0B:
        eventHandler->OnKeyDown('0');
        break;
    case 0x0C:
        eventHandler->OnKeyDown('-');
        break;
    case 0x0D:
        eventHandler->OnKeyDown('=');
        break;

    case 0x10:
        eventHandler->OnKeyDown('q');
        break;
    case 0x11:
        eventHandler->OnKeyDown('w');
        break;
    case 0x12:
        eventHandler->OnKeyDown('e');
        break;
    case 0x13:
        eventHandler->OnKeyDown('r');
        break;
    case 0x14:
        eventHandler->OnKeyDown('t');
        break;
    case 0x15:
        eventHandler->OnKeyDown('y');
        break;
    case 0x16:
        eventHandler->OnKeyDown('u');
        break;
    case 0x17:
        eventHandler->OnKeyDown('i');
        break;
    case 0x18:
        eventHandler->OnKeyDown('o');
        break;
    case 0x19:
        eventHandler->OnKeyDown('p');
        break;

    case 0x1A:
        eventHandler->OnKeyDown('[');
        break;
    case 0x1B:
        eventHandler->OnKeyDown(']');
        break;
    case 0x2B:
        eventHandler->OnKeyDown('\\');
        break;

    case 0x1E:
        eventHandler->OnKeyDown('a');
        break;
    case 0x1F:
        eventHandler->OnKeyDown('s');
        break;
    case 0x20:
        eventHandler->OnKeyDown('d');
        break;
    case 0x21:
        eventHandler->OnKeyDown('f');
        break;
    case 0x22:
        eventHandler->OnKeyDown('g');
        break;
    case 0x23:
        eventHandler->OnKeyDown('h');
        break;
    case 0x24:
        eventHandler->OnKeyDown('j');
        break;
    case 0x25:
        eventHandler->OnKeyDown('k');
        break;
    case 0x26:
        eventHandler->OnKeyDown('l');
        break;

    case 0x27:
        eventHandler->OnKeyDown(';');
        break;
    case 0x28:
        eventHandler->OnKeyDown('\'');
        break;
    case 0x29:
        eventHandler->OnKeyDown('`');
        break;

    case 0x2C:
        eventHandler->OnKeyDown('z');
        break;
    case 0x2D:
        eventHandler->OnKeyDown('x');
        break;
    case 0x2E:
        eventHandler->OnKeyDown('c');
        break;
    case 0x2F:
        eventHandler->OnKeyDown('v');
        break;
    case 0x30:
        eventHandler->OnKeyDown('b');
        break;
    case 0x31:
        eventHandler->OnKeyDown('n');
        break;
    case 0x32:
        eventHandler->OnKeyDown('m');
        break;
    case 0x33:
        eventHandler->OnKeyDown(',');
        break;
    case 0x34:
        eventHandler->OnKeyDown('.');
        break;
    case 0x35:
        eventHandler->OnKeyDown('/');
        break;

    case 0x1C:
        eventHandler->OnKeyDown('\n');
        break;
    case 0x39:
        eventHandler->OnKeyDown(' ');
        break;
// Special keus
    case 0x0E:
        eventHandler->OnKeyDown(0X0E);
        break;

    default:
    {
        printf("\nUnknown key pressed => 0x%x", key);
        break;
    }
    }

    return esp;
}