#include <drivers/vga.h>

using namespace moos::common;
using namespace moos::drivers;

VideoGraphicsArray::VideoGraphicsArray() : miscPort(0x3C2),
                                           crtcIndexPort(0x3D4),
                                           crtcDataPort(0x3D5),
                                           sequencerIndexPort(0x3C4),
                                           sequencerDataport(0x3C5),
                                           graphicsControllerIndexPort(0x3CE),
                                           graphicsControllerDataPort(0x3CF),
                                           attributeControllerIndexPort(0x3C0),
                                           attributeControllerReadPort(0x3C1),
                                           attributeControllerWritePort(0x3C0),
                                           attributeControllerResetPort(0x3DA)
{
}
VideoGraphicsArray::~VideoGraphicsArray()
{
    for (int32_t i = 0; i < 64000; i++)
    {
        lastBuffer[i] = 250;
    }
}

void VideoGraphicsArray::WriteRegisters(uint8_t *registers)
{
    miscPort.Write(*(registers++));

    for (uint8_t i = 0; i < 5; i++)
    {
        sequencerIndexPort.Write(i);
        sequencerDataport.Write(*(registers++));
    }

    crtcIndexPort.Write(0x03);
    crtcDataPort.Write(crtcDataPort.Read() | 0x80);
    crtcIndexPort.Write(0x11);
    crtcDataPort.Write(crtcDataPort.Read() & ~0x80);

    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80;

    for (uint8_t i = 0; i < 25; i++)
    {
        crtcIndexPort.Write(i);
        crtcDataPort.Write(*(registers++));
    }

    for (uint8_t i = 0; i < 9; i++)
    {
        graphicsControllerIndexPort.Write(i);
        graphicsControllerDataPort.Write(*(registers++));
    }

    for (uint8_t i = 0; i < 21; i++)
    {
        attributeControllerResetPort.Read();
        attributeControllerIndexPort.Write(i);
        attributeControllerWritePort.Write(*(registers++));
    }

    attributeControllerResetPort.Read();
    attributeControllerIndexPort.Write(0x20);
}

uint8_t *VideoGraphicsArray::GetFrameBufferSergment()
{
    graphicsControllerIndexPort.Write(0x06);
    uint8_t segmentNumber = graphicsControllerDataPort.Read() & (3 << 2);
    switch (segmentNumber)
    {
    default:
    case 0 << 2:
        return (uint8_t *)0x00000;
    case 1 << 2:
        return (uint8_t *)0xA0000;
    case 2 << 2:
        return (uint8_t *)0xB0000;
    case 3 << 2:
        return (uint8_t *)0xB8000;
    }
}

void VideoGraphicsArray::WaitForVSync() {
    // attempting to prevent 'tearing', but don't think this is right.
    // so unused for now
    uint8_t status = attributeControllerResetPort.Read() & 8;

    while (status == 0) {
        status = attributeControllerResetPort.Read() & 8;
    }
}

void VideoGraphicsArray::Finalize()
{
    for (uint32_t i = 0; i < 64000; i++)
    {
        if (lastBuffer[i] != buffer[i])
        {
            uint8_t *pixelAddress = GetFrameBufferSergment() + i;
            *pixelAddress = buffer[i];

            lastBuffer[i] = buffer[i];
        }
    }
}

void VideoGraphicsArray::PutPixel(
    int32_t x,
    int32_t y,
    uint8_t colorIndex)
{
    if (x < 0 || x >= 320 || y < 0 || y >= 200)
    {
        return;
    }
    buffer[320 * y + x] = colorIndex;
}

void VideoGraphicsArray::PutPixel(
    int32_t x,
    int32_t y,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    PutPixel(x, y, GetColorIndex(r, g, b));
}

uint8_t VideoGraphicsArray::GetColorIndex(
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    if (r == 0xA8 && g == 0x00 && b == 0x00)
    {
        return 0x04; // red
    }
    else if (r == 0x00 && g == 0xA8 && b == 0x00)
    {
        return 0x02; // green
    }
    else if (r == 0x00 && g == 0x00 && b == 0xA8)
    {
        return 0x01; // blue
    }
    else if (r == 0xFF && g == 0xFF && b == 0xFF)
    {
        return 0x1F; // white
    }
    else if (r == 0x00 && g == 0x00 && b == 0x00)
    {
        return 0x00; // black
    }
    else if (r == 0xA8 && g == 0xA8 && b == 0x00)
    {
        return 0x0E; // yellow
    }

    return 0xF; // default
}

bool VideoGraphicsArray::setMode(
    uint32_t width,
    uint32_t height,
    uint32_t colorDepth)
{
    if (!SupportsMode(width, height, colorDepth))
        return false;

    unsigned char g_320x200x256[] =
        {
            /* MISC */
            0x63,
            /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
            /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
            0xFF,
            /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
            /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00, 0x00};

    WriteRegisters(g_320x200x256);

    return true;
}

bool VideoGraphicsArray::SupportsMode(
    uint32_t width,
    uint32_t height,
    uint32_t colorDepth)
{
    return width == 320 && height == 200 && colorDepth == 8;
}