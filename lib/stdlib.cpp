#include "types.h"
#include "stdlib.h"

struct Cursor
{
    uint8_t x = 0;
    uint8_t y = 0;
};

void printf(const int8_t *str)
{
    static uint16_t *VideoMemory = (uint16_t *)0xb8000;

    static Cursor cursor;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
        case '\n':
            cursor.x = 0;
            cursor.y++;
            break;
        case '\t':
            cursor.x += 4;
            break;
        default:
            VideoMemory[80 * cursor.y + cursor.x] = (VideoMemory[80 * cursor.y + cursor.x] & 0xFF00) | str[i];
            cursor.x++;
            break;
        }

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
    }
}

int8_t *itoa(int32_t val, int32_t base)
{

    static int8_t buf[32] = {0};

    int32_t i = 30;

    buf[i--] = '\0';

    for (; val && i; --i, val /= base)

        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i + 1];
}