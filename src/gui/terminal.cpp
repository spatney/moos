#include <gui/terminal.h>
#include <hardware/video.h>
#include <common/console.h>
#include <common/strings.h>
#include <core/memory.h>

using namespace moos::gui;
using namespace moos::common;
using namespace moos::hardware;
using namespace moos::drivers;
using namespace moos::core;

const int8_t *Glyph = "\a> ";

Terminal::Terminal()
    : history(new LinkedList()),
      historyIt(history->end()),
      buffer(new int8_t[2000])
{
    x = 40;
    y = 12;
    // invertVideoMemoryAt(x, y);
    isShiftDown = false;
    isCapsLockOn = false;

    bufferCount = 0;
    promptX = 0;
    promptY = 0;
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
    auto tempBuffer = buffer;

    while (*tempBuffer != '\0')
    {
        list->AddLast(tempBuffer);

        while (*tempBuffer != ' ' && *tempBuffer != '\0')
        {
            ++tempBuffer;
        }

        if (*tempBuffer != '\0')
        {
            *tempBuffer++ = '\0';
        }
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
    case Key::Hyphen:
        return isShiftDown ? '_' : '-';
    case Key::Equal:
        return isShiftDown ? '+' : '=';
    default:
        return '\0';
    }
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
    case Key::Up:
    {
        // TODO: refactor to use for both up/down
        if (historyIt == history->end())
        {
            historyIt = history->begin();
        }
        else
        {
            ++historyIt;
            if (historyIt == history->end())
            {
                historyIt = history->begin();
            }
        }

        if (historyIt != history->end())
        {
            for (size_t i = 0; i < bufferCount; i++)
            {
                // should add a count param to Backspace()
                Console::Backspace();
            }
            bufferCount = copyToBuffer((int8_t *)*historyIt);
            Console::Write("%s", buffer);
        }
    }
    break;
    case Key::RShift:
    case Key::LShift:
        isShiftDown = true;
        break;
    case Key::CapsLock:
        isCapsLockOn = !isCapsLockOn;
        break;
    case Key::Backspace:
        if (cursor.y > promptY || cursor.x > promptX)
        {
            bufferCount--;
            Console::Backspace();
        }
        break;
    case Key::Enter:
    {
        Console::Write("\n");

        buffer[bufferCount] = '\0';

        if (StringUtil::strcmp((const int8_t *)history->PeekFirst(), buffer))
        {
            history->AddFirst(copyBuffer());
        }
        historyIt = history->end();

        auto tokens = tokenizeBuffer();

        if (!StringUtil::strcmp("clear", (const int8_t *)tokens->PeekFirst()))
        {
            Console::Clear();
        }
        else if (!StringUtil::strcmp("uname", (const int8_t *)tokens->PeekFirst()))
        {
            auto color = Console::SetColor(2);
            Console::Write("MoOS kernel v0.1\n");
            color = Console::SetColor(color);
        }
        else if (!StringUtil::strcmp("free", (const int8_t *)tokens->PeekFirst()))
        {
            auto color = Console::SetColor(6);
            auto it = ++tokens->begin();
            if (it != tokens->end() && !StringUtil::strcmp("-h", (const int8_t *)(*it)))
            {
                Console::Write("%d KB remaining in heap\n", MemoryManager::activeMemoryManager->GetFree() / 1024);
            }
            else
            {
                Console::Write("%d bytes remaining in heap\n", MemoryManager::activeMemoryManager->GetFree());
            }
            color = Console::SetColor(color);
        }
        else
        {
            if (tokens->PeekFirst() != 0)
            {
                Console::Write("command not found: '%s'\n", (const int8_t *)tokens->PeekFirst());
            }
        }

        auto copy = shared_ptr(copyBuffer());

        tokens->FreeList();
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
    promptX = StringUtil::strlen(Glyph);
}

int8_t *Terminal::copyBuffer()
{
    size_t size = 0;
    while (buffer[size++] != '\0')
        ;

    auto bufferCopy = new int8_t[size];

    for (size_t i = 0; i < size; i++)
    {
        bufferCopy[i] = buffer[i];
    }

    return bufferCopy;
}

size_t Terminal::copyToBuffer(int8_t *src)
{
    size_t i = 0;

    while (src[i] != '\0')
    {
        buffer[i] = src[i];
        i++;
    }
    buffer[i] = '\0';

    return i;
}