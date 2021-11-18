#include <drivers/keyboard.h>
#include <common/console.h>

using namespace moos::drivers;
using namespace moos::hardware;
using namespace moos::common;

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(Key)
{
}
void KeyboardEventHandler::OnKeyUp(Key)
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
    // odly not needed asleast for VMs and emulators
    while (commandPort.Read() & 0x1)
    {
        dataPort.Read();
    } /*
    commandPort.Write(0xAE); // ask keyboard to start sending interrupts
    commandPort.Write(0x20); // get current state
    uint8_t status = (dataPort.Read() | 1) & ~0x10;
    commandPort.Write(0x60); // set state
    dataPort.Write(status);

    dataPort.Write(0xF4); // activate keyboard;*/
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
        eventHandler->OnKeyDown(Key::Num1);
        break;
    case 0x03:
        eventHandler->OnKeyDown(Key::Num2);
        break;
    case 0x04:
        eventHandler->OnKeyDown(Key::Num3);
        break;
    case 0x05:
        eventHandler->OnKeyDown(Key::Num4);
        break;
    case 0x06:
        eventHandler->OnKeyDown(Key::Num5);
        break;
    case 0x07:
        eventHandler->OnKeyDown(Key::Num6);
        break;
    case 0x08:
        eventHandler->OnKeyDown(Key::Num7);
        break;
    case 0x09:
        eventHandler->OnKeyDown(Key::Num8);
        break;
    case 0x0A:
        eventHandler->OnKeyDown(Key::Num9);
        break;
    case 0x0B:
        eventHandler->OnKeyDown(Key::Num0);
        break;
    case 0x0C:
        eventHandler->OnKeyDown(Key::Hyphen);
        break;
    case 0x0D:
        eventHandler->OnKeyDown(Key::Equal);
        break;
    case 0x10:
        eventHandler->OnKeyDown(Key::Q);
        break;
    case 0x11:
        eventHandler->OnKeyDown(Key::W);
        break;
    case 0x12:
        eventHandler->OnKeyDown(Key::E);
        break;
    case 0x13:
        eventHandler->OnKeyDown(Key::R);
        break;
    case 0x14:
        eventHandler->OnKeyDown(Key::T);
        break;
    case 0x15:
        eventHandler->OnKeyDown(Key::Y);
        break;
    case 0x16:
        eventHandler->OnKeyDown(Key::U);
        break;
    case 0x17:
        eventHandler->OnKeyDown(Key::I);
        break;
    case 0x18:
        eventHandler->OnKeyDown(O);
        break;
    case 0x19:
        eventHandler->OnKeyDown(Key::P);
        break;
    case 0x1A:
        eventHandler->OnKeyDown(Key::LBracket);
        break;
    case 0x1B:
        eventHandler->OnKeyDown(Key::RBracket);
        break;
    case 0x2B:
        eventHandler->OnKeyDown(Key::Backslash);
        break;
    case 0x1E:
        eventHandler->OnKeyDown(Key::A);
        break;
    case 0x1F:
        eventHandler->OnKeyDown(Key::S);
        break;
    case 0x20:
        eventHandler->OnKeyDown(Key::D);
        break;
    case 0x21:
        eventHandler->OnKeyDown(Key::F);
        break;
    case 0x22:
        eventHandler->OnKeyDown(Key::G);
        break;
    case 0x23:
        eventHandler->OnKeyDown(Key::H);
        break;
    case 0x24:
        eventHandler->OnKeyDown(Key::J);
        break;
    case 0x25:
        eventHandler->OnKeyDown(Key::K);
        break;
    case 0x26:
        eventHandler->OnKeyDown(Key::L);
        break;
    case 0x27:
        eventHandler->OnKeyDown(Key::Semicolon);
        break;
    case 0x28:
        eventHandler->OnKeyDown(Key::Quote);
        break;
    case 0x29:
        eventHandler->OnKeyDown(Key::Tilde);
        break;
    case 0x2C:
        eventHandler->OnKeyDown(Key::Z);
        break;
    case 0x2D:
        eventHandler->OnKeyDown(Key::X);
        break;
    case 0x2E:
        eventHandler->OnKeyDown(Key::C);
        break;
    case 0x2F:
        eventHandler->OnKeyDown(Key::V);
        break;
    case 0x30:
        eventHandler->OnKeyDown(Key::B);
        break;
    case 0x31:
        eventHandler->OnKeyDown(Key::N);
        break;
    case 0x32:
        eventHandler->OnKeyDown(Key::M);
        break;
    case 0x33:
        eventHandler->OnKeyDown(Key::Comma);
        break;
    case 0x34:
        eventHandler->OnKeyDown(Key::Period);
        break;
    case 0x35:
        eventHandler->OnKeyDown(Key::Slash);
        break;
    case 0x1C:
        eventHandler->OnKeyDown(Key::Enter);
        break;
    case 0x39:
        eventHandler->OnKeyDown(Key::Space);
        break;
    case 0x0E:
        eventHandler->OnKeyDown(Key::Backspace);
        break;
    case 0x2A:
        eventHandler->OnKeyDown(Key::LShift);
        break;
    case 0x36:
        eventHandler->OnKeyDown(Key::RShift);
        break;
    case 0x3A:
        eventHandler->OnKeyDown(Key::CapsLock);
        break;
    case 0x1D:
        eventHandler->OnKeyDown(Key::LControl);
        break;
        /* case 0x1D:
        eventHandler->OnKeyDown(Key::RControl);
        break;*/
    case 0x38:
        eventHandler->OnKeyDown(Key::LAlt);
        break;
        /*case 0x38:
        eventHandler->OnKeyDown(Key::RAlt);
        break;*/
    case 0xF:
        eventHandler->OnKeyDown(Key::Tab);
        break;
    case 0x5B:
        eventHandler->OnKeyDown(Key::Menu);
        break;
    case 0x48:
        eventHandler->OnKeyDown(Key::Up);
        break;
    case 0x4D:
        eventHandler->OnKeyDown(Key::Right);
        break;
    case 0x4B:
        eventHandler->OnKeyDown(Key::Left);
        break;
    case 0x50:
        eventHandler->OnKeyDown(Key::Down);
        break;

    case 0x2A | 0x80:
        eventHandler->OnKeyUp(Key::LShift);
        break;
    case 0x36 | 0x80:
        eventHandler->OnKeyUp(Key::RShift);
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