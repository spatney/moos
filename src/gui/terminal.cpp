#include <gui/terminal.h>
#include <hardware/video.h>
#include <common/console.h>
#include <common/strings.h>

using namespace moos::gui;
using namespace moos::common;
using namespace moos::hardware;
using namespace moos::drivers;

const int8_t *Glyph = "\a> ";

Terminal::Terminal()
{
    x = 40;
    y = 12;
    // invertVideoMemoryAt(x, y);
    isShiftDown = false;
    isCapsLockOn = false;
}

Terminal::~Terminal()
{
}

void Terminal::OnMouseDown(uint8_t button)
{
    invertVideoMemoryAt(x, y);
}

void Terminal::OnMouseUp(uint8_t button)
{
    invertVideoMemoryAt(x, y);
}

void Terminal::OnMouseMove(int32_t xOffset, int32_t yOffset)
{
    xOffset /= 4;
    yOffset /= 4;

    invertVideoMemoryAt(x, y);
    x += xOffset;
    y += yOffset;

    if (x < 0)
        x = 0;
    if (x >= 80)
        x = 79;
    if (y < 0)
        y = 0;
    if (y >= 25)
        y = 24;

    invertVideoMemoryAt(x, y);
}
void Terminal::invertVideoMemoryAt(int8_t x, int8_t y)
{
    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
}

uint8_t Terminal::KeyToChar(Key key)
{
    if (key <= Key::Z && key >= Key::A)
    {
        return (key - Key::A) + (isShiftDown ^ isCapsLockOn ? 65 : 97);
    }
    if (key <= Key::Num9 && key >= Key::Num0 && !isShiftDown)
    {
        return (key - Key::Num0) + 48;
    }
    if (key == Key::Space)
    {
        return ' ';
    }

    switch (key)
    {
    case Key::Num0:
        return ')';
    case Key::Num1:
        return '!';
    case Key::Num2:
        return '@';
    case Key::Num3:
        return '#';
    case Key::Num4:
        return '$';
    case Key::Num5:
        return '%';
    case Key::Num6:
        return '^';
    case Key::Num7:
        return '&';
    case Key::Num8:
        return '*';
    case Key::Num9:
        return '(';

    default:
        break;
    }

    return '\0';
}

void Terminal::OnKeyUp(Key key)
{
    switch (key)
    {
    case Key::RShift:
    case Key::LShift:
        isShiftDown = false;
        break;

    default:
        break;
    }
}

void Terminal::OnKeyDown(Key key)
{
    auto cursor = Console::ReadCursor();
    switch (key)
    {
    case Key::RShift:
    case Key::LShift:
        isShiftDown = true;
        break;
    case Key::CapsLock:
        isCapsLockOn = !isCapsLockOn;
        break;
    case Key::Backspace:
        if (cursor.y > promptY || cursor.x > StringUtil::strlen(Glyph))
        {
            Console::Backspace();
        }

        break;

    default:
        Console::Write("%c", KeyToChar(key));
        break;
    }
}

void Terminal::Reset()
{
    Console::Clear();
    Console::Write("Welcome to MoOS!\n\n");
    drawPrompt();
}

void Terminal::drawPrompt()
{
    Console::Write(Glyph);
    promptY = Console::ReadCursor().y;
}