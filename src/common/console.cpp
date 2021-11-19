#include <common/console.h>

#include <stdarg.h>
#include <hardware/video.h>
#include <hardware/port.h>

using namespace moos::common;
using namespace moos::hardware;

static Cursor cursor;
static Port8Bit dataPort(0x3D4);
static Port8Bit commandPort(0x3D5);
static uint8_t DefaultColor = 0x03;
static uint8_t color = 0x03;

void Console::moveCursorBackByOne()
{
    cursor.x -= 1;
    if (cursor.x < 0 && cursor.y > 0)
    {
        cursor.x = 79;
        cursor.y--;
    }
    else if (cursor.x < 0)
    {
        cursor.x = 0;
    }
}

void Console::Backspace()
{
    moveCursorBackByOne();
    Write(" ");
    moveCursorBackByOne();

    updateCursor();
}

void Console::Clear()
{
    for (cursor.y = 0; cursor.y < 25; cursor.y++)
        for (cursor.x = 0; cursor.x < 80; cursor.x++)
            VideoMemory[80 * cursor.y + cursor.x] = (DefaultColor << 8) | ' ' ;
    cursor.x = 0;
    cursor.y = 0;

    enableCusor(0, 0);
    updateCursor();
}

uint8_t Console::SetColor(uint8_t c)
{
    uint8_t temp = color;
    color = c;
    return temp;
}

void Console::Write(const int8_t *message, ...)
{
    va_list args;
    va_start(args, 0);

    for (uint32_t i = 0; message[i] != '\0'; ++i)
    {
        if (cursor.x >= 80)
        {
            cursor.x = 0;
            cursor.y++;
        }

        if (cursor.y >= 25)
        {
            for (cursor.y = 0; cursor.y < 24; cursor.y++)
                for (cursor.x = 0; cursor.x < 80; cursor.x++)
                    VideoMemory[80 * cursor.y + cursor.x] = VideoMemory[80 * (cursor.y + 1) + cursor.x];

            cursor.y++;
            for (cursor.x = 0; cursor.x < 80; cursor.x++)
                VideoMemory[80 * cursor.y + cursor.x] = (color << 8) | ' ';

            cursor.x = 0;
            cursor.y -= 1;
        }

        switch (message[i])
        {
        case '\n':
            for (; cursor.x < 80; cursor.x++)
                VideoMemory[80 * cursor.y + cursor.x] = (color << 8) | ' ';
            cursor.y++;
            cursor.x = 0;
            Write(" ");
            for (; cursor.x < 80; cursor.x++)
                VideoMemory[80 * cursor.y + cursor.x] = (color << 8) | ' ';
            cursor.x = 0;
            break;
        case '\t':
        {
            int8_t tempX = cursor.x;
            for (; cursor.x < 80; cursor.x++)
                VideoMemory[80 * cursor.y + cursor.x] = (color << 8) | ' ';
            cursor.x = tempX;
            cursor.x += 4;
            break;
        }
        case '%':
            if (message[i + 1] == 'd')
            {
                Write(itoa(va_arg(args, int32_t), /*base*/ 10));
                i++;
                break;
            }
            else if (message[i + 1] == 'x')
            {
                Write(itoa(va_arg(args, int32_t), /*base*/ 16));
                i++;
                break;
            }
            else if (message[i + 1] == 'o')
            {
                Write(itoa(va_arg(args, int32_t), /*base*/ 8));
                i++;
                break;
            }
            else if (message[i + 1] == 's')
            {
                Write(va_arg(args, int8_t *));
                i++;
                break;
            }
            else if (message[i + 1] == 'c')
            {
                int32_t c = va_arg(args, int32_t);
                int8_t cStr[2];
                cStr[0] = c;
                cStr[1] = '\0';
                Write(cStr);
                i++;
                break;
            }

        default:
            VideoMemory[80 * cursor.y + cursor.x] = (color << 8) | message[i];
            cursor.x++;
            break;
        }
    }

    updateCursor();
}

int8_t *Console::itoa(int32_t val, const int32_t base)
{
    static int8_t Representation[] = "0123456789ABCDEF";
    static int8_t buffer[50];
    int8_t *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    int8_t counter = 0;

    do
    {
        if (base == 10 && counter++ == 3)
        {
            *--ptr = ',';
            counter = 0;
        }
        else
        {
            *--ptr = Representation[val % base];
            val /= base;
        }
    } while (val != 0);

    return ptr;
}

void Console::Sleep(int32_t seconds)
{
    auto startTicks = Console::ticks;

    while ((Console::ticks - startTicks) < (seconds * 20))
        ;
}

Cursor Console::ReadCursor()
{
    return cursor;
}

void Console::enableCusor(
    common::uint8_t cursor_start,
    common::uint8_t cursor_end)
{
    dataPort.Write(0x0A);
    commandPort.Write((commandPort.Read() & 0xC0) | cursor_start);

    dataPort.Write(0x0B);
    commandPort.Write((commandPort.Read() & 0xE0) | cursor_end);
}

void Console::updateCursor()
{
    uint16_t pos = (cursor.y + 1) * 80 + cursor.x;

    dataPort.Write(0x0F);
    commandPort.Write((uint8_t)(pos & 0xFF));
    dataPort.Write(0x0E);
    commandPort.Write((uint8_t)((pos >> 8) & 0xFF));
}

void Console::disableCursor()
{
    dataPort.Write(0x0A);
    commandPort.Write(0x20);
}