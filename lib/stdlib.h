#ifndef __STDLIB_H
#define __STDLIB_H

#include "types.h"

int8_t* itoa(int32_t val, int32_t base);
void printf(const int8_t* message, ...);
void wait(int32_t seconds);

#endif