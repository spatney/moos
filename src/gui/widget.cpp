#include <gui/widget.h>

using namespace moos::gui;
using namespace moos::common;

Widget::Widget(
    Widget *parent,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    int32_t r,
    int32_t g,
    int32_t b)
{
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->r = r;
    this->g = g;
    this->b = b;
    this->focusable = true;
}

Widget::~Widget()
{
}

void Widget::GetFocus(Widget *widget)
{
    if (parent != 0)
    {
        parent->GetFocus(widget);
    }
}

void Widget::ModelToScreen(
    int32_t &x,
    int32_t &y)
{
    if (parent != 0)
    {
        parent->ModelToScreen(x, y);

        x += this->x;
        y += this->y;
    }
}

void Widget::Draw(GraphicsContext *gc)
{
    int32_t X = 0;
    int32_t Y = 0;

    ModelToScreen(X, Y);

    gc->FillRectangle(X, Y, w, h, r, g, b);
}

void Widget::OnMouseDown(
    int32_t x,
    int32_t y)
{
    if (focusable)
    {
        GetFocus(this);
    }
}

void Widget::OnMouseUp(
    int32_t x,
    int32_t y)
{
}

void Widget::OnMouseMove(
    int32_t oldX,
    int32_t oldY,
    int32_t newX,
    int32_t newY)
{
}

void Widget::OnKeyDown(char *str)
{
}

void Widget::OnKeyUp(char *str)
{
}

bool Widget::ContainsCoordinate(
    int32_t x,
    int32_t y)
{
    return false;
}

CompositeWidget::CompositeWidget(
    Widget *parent,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    int32_t r,
    int32_t g,
    int32_t b)
    : Widget(parent, x, y, w, h, r, g, b)
{
    focussedChild = 0;
    numChildren = 0;
}
CompositeWidget::~CompositeWidget()
{
}

void CompositeWidget::Draw(GraphicsContext *gc)
{
    Widget::Draw(gc);
    for (int32_t i = numChildren - 1; i >= 0; --i)
    {
        children[i]->Draw(gc);
    }
}

void CompositeWidget::GetFocus(Widget *widget)
{
    this->focussedChild = widget;
    if (parent != 0)
    {
        parent->GetFocus(this);
    }
}

void CompositeWidget::OnMouseDown(
    int32_t x,
    int32_t y)
{
    for (int32_t i = 0; i < numChildren; --i)
    {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y))
        {
            children[i]->OnMouseDown(x - this->x, y - this->y);
            break;
        }
    }
}

void CompositeWidget::OnMouseUp(
    int32_t x,
    int32_t y)
{
    for (int32_t i = 0; i < numChildren; --i)
    {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y))
        {
            children[i]->OnMouseUp(x - this->x, y - this->y);
            break;
        }
    }
}

void CompositeWidget::OnMouseMove(
    int32_t oldX,
    int32_t oldY,
    int32_t newX,
    int32_t newY)
{
    int32_t eventFiredOnChild = -1;

    for (int32_t i = 0; i < numChildren; --i)
    {
        if (children[i]->ContainsCoordinate(oldX - this->x, oldY - this->y))
        {
            children[i]->OnMouseMove(oldX - this->x, oldY - this->y, newX - this->x, newY - this->y);
            eventFiredOnChild = i;
            break;
        }
    }

    for (int32_t i = 0; i < numChildren; --i)
    {
        if (children[i]->ContainsCoordinate(newX - this->x, newY - this->y))
        {
            if (i != eventFiredOnChild)
            {
                children[i]->OnMouseMove(oldX - this->x, oldY - this->y, newX - this->x, newY - this->y);
            }
            break;
        }
    }
}

void CompositeWidget::OnKeyDown(char *str)
{
    if (focussedChild != 0)
    {
        focussedChild->OnKeyDown(str);
    }
}

void CompositeWidget::OnKeyUp(char *str)
{
    if (focussedChild != 0)
    {
        focussedChild->OnKeyUp(str);
    }
}