#include <gui/window.h>

using namespace moos::gui;
using namespace moos::common;
using namespace moos::drivers;

Window::Window(
    Widget *parent,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    int32_t r,
    int32_t g,
    int32_t b)
    : CompositeWidget(parent, x, y, w, h, r, g, b)
{
    Dragging = false;
}

Window::~Window()
{
}

void Window::OnMouseDown(
    int32_t x,
    int32_t y,
    uint8_t button)
{
    Dragging = button == 1;

    if (Dragging)
    {
        ((CompositeWidget *)parent)->BringChildToFront(this);
    }
}

void Window::OnMouseUp(
    int32_t x,
    int32_t y,
    uint8_t button)
{
    Dragging = false;
}

void Window::OnMouseMove(
    int32_t oldX,
    int32_t oldY,
    int32_t newX,
    int32_t newY)
{
    if (Dragging)
    {
        this->x += newX - oldX;
        this->y += newY - oldY;
    }
    CompositeWidget::OnMouseMove(oldX, oldY, newX, newY);
}

void Window::OnMouseLeave()
{
}

void Window::OnMouseEnter()
{
}
