#include <drivers/keyboard.h>
#include <common/console.h>

using namespace moos::drivers;
using namespace moos::hardware;
using namespace moos::common;

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
    isShiftDown = false;
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

    if (eventHandler == 0)
    {
        return esp;
    }

    if (key == 0xFA || key == 0x45)
    {
        return esp;
    }

    switch (key)
    {
    case 0x02:
        eventHandler->OnKeyDown(isShiftDown ? '!' : '1');
        break;
    case 0x03:
        eventHandler->OnKeyDown(isShiftDown ? '@' : '2');
        break;
    case 0x04:
        eventHandler->OnKeyDown(isShiftDown ? '#' : '3');
        break;
    case 0x05:
        eventHandler->OnKeyDown(isShiftDown ? '$' : '4');
        break;
    case 0x06:
        eventHandler->OnKeyDown(isShiftDown ? '%' : '5');
        break;
    case 0x07:
        eventHandler->OnKeyDown(isShiftDown ? '^' : '6');
        break;
    case 0x08:
        eventHandler->OnKeyDown(isShiftDown ? '&' : '7');
        break;
    case 0x09:
        eventHandler->OnKeyDown(isShiftDown ? '*' : '8');
        break;
    case 0x0A:
        eventHandler->OnKeyDown(isShiftDown ? '(' : '9');
        break;
    case 0x0B:
        eventHandler->OnKeyDown(isShiftDown ? ')' : '0');
        break;
    case 0x0C:
        eventHandler->OnKeyDown(isShiftDown ? '_' : '-');
        break;
    case 0x0D:
        eventHandler->OnKeyDown(isShiftDown ? '+' : '=');
        break;

    case 0x10:
        eventHandler->OnKeyDown(isShiftDown ? 'Q' :'q');
        break;
    case 0x11:
        eventHandler->OnKeyDown(isShiftDown ? 'W' :'w');
        break;
    case 0x12:
        eventHandler->OnKeyDown(isShiftDown ? 'E' :'e');
        break;
    case 0x13:
        eventHandler->OnKeyDown(isShiftDown ? 'R' :'r');
        break;
    case 0x14:
        eventHandler->OnKeyDown(isShiftDown ? 'T' :'t');
        break;
    case 0x15:
        eventHandler->OnKeyDown(isShiftDown ? 'Y' :'y');
        break;
    case 0x16:
        eventHandler->OnKeyDown(isShiftDown ? 'U' :'u');
        break;
    case 0x17:
        eventHandler->OnKeyDown(isShiftDown ? 'I' :'i');
        break;
    case 0x18:
        eventHandler->OnKeyDown(isShiftDown ? 'O' :'o');
        break;
    case 0x19:
        eventHandler->OnKeyDown(isShiftDown ? 'P' :'p');
        break;

    case 0x1A:
        eventHandler->OnKeyDown(isShiftDown ? '{' :'[');
        break;
    case 0x1B:
        eventHandler->OnKeyDown(isShiftDown ? '}' :']');
        break;
    case 0x2B:
        eventHandler->OnKeyDown(isShiftDown ? '|' :'\\');
        break;

    case 0x1E:
        eventHandler->OnKeyDown(isShiftDown ? 'A' :'a');
        break;
    case 0x1F:
        eventHandler->OnKeyDown(isShiftDown ? 'S' :'s');
        break;
    case 0x20:
        eventHandler->OnKeyDown(isShiftDown ? 'D' :'d');
        break;
    case 0x21:
        eventHandler->OnKeyDown(isShiftDown ? 'F' :'f');
        break;
    case 0x22:
        eventHandler->OnKeyDown(isShiftDown ? 'G' :'g');
        break;
    case 0x23:
        eventHandler->OnKeyDown(isShiftDown ? 'H' :'h');
        break;
    case 0x24:
        eventHandler->OnKeyDown(isShiftDown ? 'J' :'j');
        break;
    case 0x25:
        eventHandler->OnKeyDown(isShiftDown ? 'K' :'k');
        break;
    case 0x26:
        eventHandler->OnKeyDown(isShiftDown ? 'L' :'l');
        break;

    case 0x27:
        eventHandler->OnKeyDown(isShiftDown ? ':' :';');
        break;
    case 0x28:
        eventHandler->OnKeyDown(isShiftDown ? '"' :'\'');
        break;
    case 0x29:
        eventHandler->OnKeyDown(isShiftDown ? '~' :'`');
        break;

    case 0x2C:
        eventHandler->OnKeyDown(isShiftDown ? 'Z' :'z');
        break;
    case 0x2D:
        eventHandler->OnKeyDown(isShiftDown ? 'X' :'x');
        break;
    case 0x2E:
        eventHandler->OnKeyDown(isShiftDown ? 'C' :'c');
        break;
    case 0x2F:
        eventHandler->OnKeyDown(isShiftDown ? 'V' :'v');
        break;
    case 0x30:
        eventHandler->OnKeyDown(isShiftDown ? 'B' :'b');
        break;
    case 0x31:
        eventHandler->OnKeyDown(isShiftDown ? 'N' :'n');
        break;
    case 0x32:
        eventHandler->OnKeyDown(isShiftDown ? 'M' :'m');
        break;
    case 0x33:
        eventHandler->OnKeyDown(isShiftDown ? '<' : ',');
        break;
    case 0x34:
        eventHandler->OnKeyDown(isShiftDown ? '>' : '.');
        break;
    case 0x35:
        eventHandler->OnKeyDown(isShiftDown ? '?' : '/');
        break;

    case 0x1C:
        eventHandler->OnKeyDown('\n');
        break;
    case 0x39:
        eventHandler->OnKeyDown(' ');
        break;
        // Special keys
    case 0x0E:
        eventHandler->OnKeyDown(0X0E);
        break;

    case 0x2A:
    case 0x36:
        isShiftDown = true;
        break;

    case 0xB6:
    case 0XAA:
        isShiftDown = false;
        break;

    default:
    {
        if (key >= 0x80)
        {
            // ignore key up
            break;
        }
        Console::Write("\nUnknown key pressed => 0x%x", key);
        break;
    }
    }

    return esp;
}