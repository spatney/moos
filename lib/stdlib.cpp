#include "types.h"
#include "stdlib.h"
#include "hardware.h"

struct Cursor
{
    uint8_t x = 0;
    uint8_t y = 0;
};

void printf(const int8_t *message)
{
    static Cursor cursor;

    for (uint16_t size = 0; message[size] != '\0'; ++size)
    {
        switch (message[size])
        {
        case '\n':
            cursor.x = 0;
            cursor.y++;
            break;
        case '\t':
            cursor.x += 4;
            break;
        default:
            VideoMemory[80 * cursor.y + cursor.x] = (VideoMemory[80 * cursor.y + cursor.x] & 0xFF00) | message[size];
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
    int32_t size = 30;

    buf[size--] = '\0';

    for (; val && size; --size, val /= base)
        buf[size] = "0123456789abcdef"[val % base];

    return &buf[size + 1];
}