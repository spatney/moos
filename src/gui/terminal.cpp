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

    buffer = new uint8_t[400];
    bufferCount = 0;
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

LinkedList *Terminal::tokenizeBuffer()
{
    auto list = new LinkedList();
    uint8_t *tempBuffer = buffer;

    while (*tempBuffer != '\0')
    {
        auto token = new Token();
        token->str = tempBuffer;

        while (*tempBuffer != ' ' && *tempBuffer != '\0')
        {
            ++tempBuffer;
        }

        *tempBuffer++ = '\0';

        list->AddLast(token);
    }

    return list;
}

uint8_t Terminal::keyToChar(Key key)
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
    case Key::Period:
        return isShiftDown ? '>' : '.';
    case Key::Comma:
        return isShiftDown ? '<' : ',';
    case Key::Slash:
        return isShiftDown ? '?' : '/';
    case Key::Backslash:
        return isShiftDown ? '|' : '\\';
    case Key::Tab:
        return '\t';
    case Key::Enter:
        return '\n';
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
            bufferCount--;
            Console::Backspace();
        }
        break;
    case Key::Enter:
    {
        Console::Write("\n");

        buffer[bufferCount] = '\0';
        auto tokens = tokenizeBuffer();

        if (!StringUtil::strcmp("clear", (const char *)(((Token *)tokens->PeekFirst())->str)))
        {
            Console::Clear();
        }
        else
        {
            // for debugging
            /*for (auto it = tokens->begin(), end = tokens->end(); it != end; ++it)
            {
                auto data = *it;
                common::Console::Write("%s => ", ((Token *)data)->str);
            }
            Console::Write("END");*/
            Console::Write("'%s' command not supported\n", (const char *)(((Token *)tokens->PeekFirst())->str));
        }

        drawPrompt();
        bufferCount = 0;
    }
    break;

    default:
        auto c = keyToChar(key);
        if (c != '\0')
        {
            buffer[bufferCount++] = c;
            Console::Write("%c", c);
        }
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