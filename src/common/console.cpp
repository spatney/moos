#include <common/console.h>
#include <stdarg.h>
#include <common/types.h>
#include <hardware/video.h>

using namespace moos::common;
using namespace moos::hardware;

struct Cursor
{
    int8_t x = 0;
    int8_t y = 0;
};

static Cursor cursor;

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
            for (cursor.y = 0; cursor.y < 25; cursor.y++)
                for (cursor.x = 0; cursor.x < 80; cursor.x++)
                    VideoMemory[80 * cursor.y + cursor.x] = (VideoMemory[80 * cursor.y + cursor.x] & 0xFF00) | ' ';
            cursor.x = 0;
            cursor.y = 0;
        }

        switch (message[i])
        {
        case '\n':
            cursor.x = 0;
            cursor.y++;
            break;
        case '\t':
            cursor.x += 4;
            break;

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
            VideoMemory[80 * cursor.y + cursor.x] = (VideoMemory[80 * cursor.y + cursor.x] & 0xFF00) | message[i];
            cursor.x++;
            break;
        }
    }
}

int8_t *Console::itoa(int32_t val, const int32_t base)
{
    static int8_t Representation[] = "0123456789abcdef";
    static int8_t buffer[50];
    int8_t *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[val % base];
        val /= base;
    } while (val != 0);

    return ptr;
}

void Console::Sleep(int32_t seconds)
{
    static int32_t wait_loop0 = 4000;
    static int32_t wait_loop1 = 6000;
    int32_t i, j, k;
    for (i = 0; i < seconds; i++)
    {
        for (j = 0; j < wait_loop0; j++)
        {
            for (k = 0; k < wait_loop1; k++)
            {
                int32_t volatile t = 120 * j * i + k;
                t = t + 5;
            }
        }
    }
}