#include <gui/graphics.h>

using namespace moos::common;
using namespace moos::drivers;
using namespace moos::gui;

GraphicsContext::GraphicsContext()
{
    vga.setMode(320, 200, 8);
}

GraphicsContext::~GraphicsContext()
{
}

void GraphicsContext::FillRectangle(
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    int8_t r,
    int8_t g,
    int8_t b)
{
    for (int32_t Y = 0; Y < h; Y++)
    {
        for (int32_t X = 0; X < w; X++)
        {
            vga.PutPixel(X + x, Y + y, r, g, b);
        }
    }
}

int32_t GraphicsContext::abs(int32_t n)
{
    if (n < 0)
    {
        return n * -1;
    }
    return n;
}

void GraphicsContext::DrawLine(
    int32_t X1,
    int32_t Y1,
    int32_t X2,
    int32_t Y2,
    int8_t r,
    int8_t g,
    int8_t b)
{
    if (X1 > X2)
    {
        int32_t temp = X2;
        X2 = X1;
        X1 = temp;
    }

    if (Y1 > Y2)
    {
        int32_t temp = Y2;
        Y2 = Y1;
        Y1 = temp;
    }

    int32_t dx = X2 - X1;
    int32_t dy = Y2 - Y1;

    int32_t x = X1;
    int32_t y = Y1;


    if (abs(dx) > abs(dy))
    {
        vga.PutPixel(x, y, r, g, b);
        int pk = (2 * abs(dy)) - abs(dx);

        for (int i = 0; i < abs(dx); i++)
        {
            x = x + 1;
            if (pk < 0)
                pk = pk + (2 * abs(dy));
            else
            {
                y = y + 1;
                pk = pk + (2 * abs(dy)) - (2 * abs(dx));
            }
            vga.PutPixel(x, y, r, g, b);
        }
    }
    else
    {

        vga.PutPixel(x, y, r, g, b);
        int pk = (2 * abs(dx)) - abs(dy);

        for (int i = 0; i < abs(dy); i++)
        {
            y = y + 1;
            if (pk < 0)
                pk = pk + (2 * abs(dx));
            else
            {
                x = x + 1;
                pk = pk + (2 * abs(dx)) - (2 * abs(dy));
            }

            vga.PutPixel(x, y, r, g, b);
        }
    }
}