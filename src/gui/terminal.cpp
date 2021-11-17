#include <gui/terminal.h>
#include <hardware/video.h>
#include <common/console.h>
#include <common/strings.h>

using namespace moos::gui;
using namespace moos::common;
using namespace moos::hardware;

const int8_t* Glyph = "\a> ";

Terminal::Terminal()
{
    x = 40;
    y = 12;
    // invertVideoMemoryAt(x, y);
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

void Terminal::OnKeyDown(int8_t c)
{
    if (c == 0X0E)
    {
        auto cursor = Console::ReadCursor();

        if (cursor.y > promptY || cursor.x > StringUtil::strlen(Glyph))
        {
            Console::Backspace();
        }
    }
    else
    {
        Console::Write("%c", c);
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