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

    case 0x02 | 0x80:
        eventHandler->OnKeyUp(Key::Num1);
        break;
    case 0x03 | 0x80:
        eventHandler->OnKeyUp(Key::Num2);
        break;
    case 0x04 | 0x80:
        eventHandler->OnKeyUp(Key::Num3);
        break;
    case 0x05 | 0x80:
        eventHandler->OnKeyUp(Key::Num4);
        break;
    case 0x06 | 0x80:
        eventHandler->OnKeyUp(Key::Num5);
        break;
    case 0x07 | 0x80:
        eventHandler->OnKeyUp(Key::Num6);
        break;
    case 0x08 | 0x80:
        eventHandler->OnKeyUp(Key::Num7);
        break;
    case 0x09 | 0x80:
        eventHandler->OnKeyUp(Key::Num8);
        break;
    case 0x0A | 0x80:
        eventHandler->OnKeyUp(Key::Num9);
        break;
    case 0x0B | 0x80:
        eventHandler->OnKeyUp(Key::Num0);
        break;
    case 0x0C | 0x80:
        eventHandler->OnKeyUp(Key::Hyphen);
        break;
    case 0x0D | 0x80:
        eventHandler->OnKeyUp(Key::Equal);
        break;
    case 0x10 | 0x80:
        eventHandler->OnKeyUp(Key::Q);
        break;
    case 0x11 | 0x80:
        eventHandler->OnKeyUp(Key::W);
        break;
    case 0x12 | 0x80:
        eventHandler->OnKeyUp(Key::E);
        break;
    case 0x13 | 0x80:
        eventHandler->OnKeyUp(Key::R);
        break;
    case 0x14 | 0x80:
        eventHandler->OnKeyUp(Key::T);
        break;
    case 0x15 | 0x80:
        eventHandler->OnKeyUp(Key::Y);
        break;
    case 0x16 | 0x80:
        eventHandler->OnKeyUp(Key::U);
        break;
    case 0x17 | 0x80:
        eventHandler->OnKeyUp(Key::I);
        break;
    case 0x18 | 0x80:
        eventHandler->OnKeyUp(O);
        break;
    case 0x19 | 0x80:
        eventHandler->OnKeyUp(Key::P);
        break;
    case 0x1A | 0x80:
        eventHandler->OnKeyUp(Key::LBracket);
        break;
    case 0x1B | 0x80:
        eventHandler->OnKeyUp(Key::RBracket);
        break;
    case 0x2B | 0x80:
        eventHandler->OnKeyUp(Key::Backslash);
        break;
    case 0x1E | 0x80:
        eventHandler->OnKeyUp(Key::A);
        break;
    case 0x1F | 0x80:
        eventHandler->OnKeyUp(Key::S);
        break;
    case 0x20 | 0x80:
        eventHandler->OnKeyUp(Key::D);
        break;
    case 0x21 | 0x80:
        eventHandler->OnKeyUp(Key::F);
        break;
    case 0x22 | 0x80:
        eventHandler->OnKeyUp(Key::G);
        break;
    case 0x23 | 0x80:
        eventHandler->OnKeyUp(Key::H);
        break;
    case 0x24 | 0x80:
        eventHandler->OnKeyUp(Key::J);
        break;
    case 0x25 | 0x80:
        eventHandler->OnKeyUp(Key::K);
        break;
    case 0x26 | 0x80:
        eventHandler->OnKeyUp(Key::L);
        break;
    case 0x27 | 0x80:
        eventHandler->OnKeyUp(Key::Semicolon);
        break;
    case 0x28 | 0x80:
        eventHandler->OnKeyUp(Key::Quote);
        break;
    case 0x29 | 0x80:
        eventHandler->OnKeyUp(Key::Tilde);
        break;
    case 0x2C | 0x80:
        eventHandler->OnKeyUp(Key::Z);
        break;
    case 0x2D | 0x80:
        eventHandler->OnKeyUp(Key::X);
        break;
    case 0x2E | 0x80:
        eventHandler->OnKeyUp(Key::C);
        break;
    case 0x2F | 0x80:
        eventHandler->OnKeyUp(Key::V);
        break;
    case 0x30 | 0x80:
        eventHandler->OnKeyUp(Key::B);
        break;
    case 0x31 | 0x80:
        eventHandler->OnKeyUp(Key::N);
        break;
    case 0x32 | 0x80:
        eventHandler->OnKeyUp(Key::M);
        break;
    case 0x33 | 0x80:
        eventHandler->OnKeyUp(Key::Comma);
        break;
    case 0x34 | 0x80:
        eventHandler->OnKeyUp(Key::Period);
        break;
    case 0x35 | 0x80:
        eventHandler->OnKeyUp(Key::Slash);
        break;
    case 0x1C | 0x80:
        eventHandler->OnKeyUp(Key::Enter);
        break;
    case 0x39 | 0x80:
        eventHandler->OnKeyUp(Key::Space);
        break;
    case 0x0E | 0x80:
        eventHandler->OnKeyUp(Key::Backspace);
        break;
    case 0x2A | 0x80:
        eventHandler->OnKeyUp(Key::LShift);
        break;
    case 0x36 | 0x80:
        eventHandler->OnKeyUp(Key::RShift);
        break;
    case 0x3A | 0x80:
        eventHandler->OnKeyUp(Key::CapsLock);
        break;
    case 0x1D | 0x80:
        eventHandler->OnKeyUp(Key::LControl);
        break;
    case 0x38 | 0x80:
        eventHandler->OnKeyUp(Key::LAlt);
        break;
    case 0xF | 0x80:
        eventHandler->OnKeyUp(Key::Tab);
        break;
    case 0x5B | 0x80:
        eventHandler->OnKeyUp(Key::Menu);
        break;
    case 0x48 | 0x80:
        eventHandler->OnKeyUp(Key::Up);
        break;
    case 0x4D | 0x80:
        eventHandler->OnKeyUp(Key::Right);
        break;
    case 0x4B | 0x80:
        eventHandler->OnKeyUp(Key::Left);
        break;
    case 0x50 | 0x80:
        eventHandler->OnKeyUp(Key::Down);
        break;

    default:
        if (key >= 0x80)
        {
            // ignore key up
            break;
        }
        Console::Write("\nUnknown key pressed => 0x%x", key);
        break;
    }

    return esp;
}