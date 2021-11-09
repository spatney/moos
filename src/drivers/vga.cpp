#include <drivers/vga.h>

using namespace moos::common;
using namespace moos::drivers;

VideoGraphicsArray::VideoGraphicsArray()
{
}
VideoGraphicsArray::~VideoGraphicsArray()
{
}

void VideoGraphicsArray::WriteRegisters(uint8_t *registers)
{
}

uint8_t VideoGraphicsArray::*GetFrameBufferSergment()
{
}

void VideoGraphicsArray::PutPixel(
    uint32_t x,
    uint32_t y,
    uint8_t colorIndex)
{
}

uint8_t VideoGraphicsArray::GetColorIndex(
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
}

bool VideoGraphicsArray::setMode(
    uint32_t width,
    uint32_t height,
    uint32_t colorDepth)
{
}

bool VideoGraphicsArray::SupportsMode(
    uint32_t width,
    uint32_t height,
    uint32_t colorDepth)
{
}

void VideoGraphicsArray::PutPixel(
    uint32_t x,
    uint32_t y,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
}