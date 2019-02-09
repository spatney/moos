#include <stdarg.h>
#include "types.h"
#include "stdlib.h"
#include "hardware.h"

struct Cursor
{
    uint8_t x = 0;
    uint8_t y = 0;
};

void printf(const int8_t* message, ...)
{
    static Cursor cursor;

    va_list valist;
    va_start(valist, 0);

    for (uint16_t i = 0; message[i] != '\0'; ++i)
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
                printf(itoa(va_arg(valist, int32_t), /*base*/ 10));
                i++;
                break;
            }
            else if (message[i + 1] == '#')
            {
                printf(itoa(va_arg(valist, int32_t), /*base*/ 16));
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

int8_t* itoa(int32_t val, int32_t base)
{
    static int8_t buf[32] = {0};
    int32_t size = 30;

    buf[size--] = '\0';

    for (; val && size; --size, val /= base)
        buf[size] = "0123456789abcdef"[val % base];

    return &buf[size + 1];
}

void wait(int32_t seconds)
{
    static int16_t wait_loop0 = 500;
    static int16_t wait_loop1 = 6000;
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