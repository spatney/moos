#include <gui/desktop.h>

using namespace moos::gui;
using namespace moos::common;
using namespace moos::drivers;

Desktop::Desktop(
    int32_t w,
    int32_t h,
    int32_t r,
    int32_t g,
    int32_t b)
    : CompositeWidget(0, 0, 0, w, h, r, g, b),
      MouseEventHandler()
{
    MouseX = LastMouseX = w / 2;
    MouseY = LastMouseY = h * 3/5;
}

Desktop::~Desktop()
{
}

void Desktop::Draw(gui::GraphicsContext *gc)
{
    CompositeWidget::Draw(gc);

    uint8_t rSize = 6;
    uint8_t size = 2;
    int32_t centerX = w / 2;
    int32_t centerY = h / 2;

    gc->FillRectangle(centerX - rSize, centerY - rSize, rSize, rSize, 0xA8, 0x00, 0x00);
    gc->FillRectangle(centerX + 1, centerY - rSize, rSize, rSize, 0x00, 0xA8, 0x00);
    gc->FillRectangle(centerX - rSize, centerY + 1, rSize, rSize, 0x00, 0x00, 0xA8);
    gc->FillRectangle(centerX + 1, centerY + 1, rSize, rSize, 0xA8, 0xA8, 0x00);
    gc->DrawLine(centerX, centerY - rSize, centerX, centerY + rSize, 0xFF, 0xFF, 0xFF);
    gc->DrawLine(centerX - rSize, centerY, centerX + rSize, centerY, 0xFF, 0xFF, 0xFF);

    gc->DrawLine(MouseX, MouseY - size, MouseX, MouseY + size, 0xA8, 0x00, 0x00);
    gc->DrawLine(MouseX - size, MouseY, MouseX + size, MouseY, 0xA8, 0x00, 0x00);

    LastMouseX = MouseX;
    LastMouseY = MouseY;

    gc->Finalize();
}

void Desktop::OnMouseDown(uint8_t button)
{
    CompositeWidget::OnMouseDown(MouseX, MouseY, button);
}

void Desktop::OnMouseUp(uint8_t button)
{
    CompositeWidget::OnMouseUp(MouseX, MouseY, button);
}

void Desktop::OnMouseMove(
    int32_t xOffset,
    int32_t yOffset)
{
    //xOffset /= 2;
    //yOffset /= 2;

    int32_t newMouseX = MouseX + xOffset;
    if (newMouseX < 0)
        newMouseX = 0;
    if (newMouseX >= w)
        newMouseX = w - 1;

    int32_t newMouseY = MouseY + yOffset;
    if (newMouseY < 0)
        newMouseY = 0;
    if (newMouseY >= h)
        newMouseY = h - 1;

    CompositeWidget::OnMouseMove(MouseX, MouseY, newMouseX, newMouseY);

    MouseX = newMouseX;
    MouseY = newMouseY;
}