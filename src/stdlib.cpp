#include "types.h"
#include "stdlib.h"

int32_t cursor=0;

void printf(const int8_t * msg) {
    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for(uint32_t i=0; msg[i] != '\0'; ++i) {
        VideoMemory[cursor] = (VideoMemory[cursor] & 0xFF00 | msg[i]);
        cursor++;
    }
}

int8_t* itoa(int32_t val, int32_t base){

    static int8_t buf[32] = {0};

    int32_t i = 30;

    buf[i--] = '\0';

    for(; val && i ; --i, val /= base)

        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i+1];

}