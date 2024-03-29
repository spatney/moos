#include <gui/widget.h>

using namespace moos::gui;
using namespace moos::common;
using namespace moos::drivers;

Widget::Widget(
    Widget *parent,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    int32_t r,
    int32_t g,
    int32_t b) : KeyboardEventHandler()
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
    }

    x += this->x;
    y += this->y;
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
    int32_t y,
    uint8_t button)
{
    if (focusable)
    {
        GetFocus(this);
    }
}

void Widget::OnMouseUp(
    int32_t x,
    int32_t y,
    uint8_t button)
{
}

void Widget::OnMouseMove(
    int32_t oldX,
    int32_t oldY,
    int32_t newX,
    int32_t newY)
{
}

void Widget::OnMouseLeave()
{
}

void Widget::OnMouseEnter()
{
}

bool Widget::ContainsCoordinate(
    int32_t x,
    int32_t y)
{
    return this->x <= x 
    && x < this->x + this->w 
    && this->y <= y 
    && y < this->y + this->h;
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

bool CompositeWidget::AddChildWidget(Widget *child)
{
    if (numChildren >= 100)
    {
        return false;
    }
    children[numChildren] = child;
    numChildren++;
    return true;
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
    int32_t y,
    uint8_t button)
{
    for (int32_t i = 0; i < numChildren; ++i)
    {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y))
        {
            children[i]->OnMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::OnMouseUp(
    int32_t x,
    int32_t y,
    uint8_t button)
{
    for (int32_t i = 0; i < numChildren; ++i)
    {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y))
        {
            children[i]->OnMouseUp(x - this->x, y - this->y, button);
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

    for (int32_t i = 0; i < numChildren; ++i)
    {
        if (children[i]->ContainsCoordinate(oldX - this->x, oldY - this->y) && children[i]->ContainsCoordinate(newX - this->x, newY - this->y))
        {
            children[i]->OnMouseMove(oldX - this->x, oldY - this->y, newX - this->x, newY - this->y);
            break;
        }
        else if (children[i]->ContainsCoordinate(oldX - this->x, oldY - this->y) && !children[i]->ContainsCoordinate(newX - this->x, newY - this->y))
        {
            children[i]->OnMouseLeave();
        }
        else if (!children[i]->ContainsCoordinate(oldX - this->x, oldY - this->y) && children[i]->ContainsCoordinate(newX - this->x, newY - this->y))
        {
            children[i]->OnMouseEnter();
        }
    }
}

void CompositeWidget::OnMouseLeave()
{
}

void CompositeWidget::OnMouseEnter()
{
}

void CompositeWidget::OnKeyDown(Key c)
{
    if (focussedChild != 0)
    {
        focussedChild->OnKeyDown(c);
    }
}

void CompositeWidget::OnKeyUp(Key c)
{
    if (focussedChild != 0)
    {
        focussedChild->OnKeyUp(c);
    }
}

void CompositeWidget::BringChildToFront(Widget *child)
{
    uint32_t myIndex = 0;

    for (uint32_t i = 0; i < numChildren; i++)
    {
        if (children[i] == child)
        {
            myIndex = i;
            break;
        }
    }

    if (myIndex != 0)
    {
        Widget *temp = children[0];
        children[0] = children[myIndex];
        children[myIndex] = temp;
    }
}