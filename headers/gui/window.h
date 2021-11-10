#ifndef __MOOS__GUI__Window_H
#define __MOOS__GUI__Window_H

#include <gui/widget.h>
#include <drivers/mouse.h>

namespace moos
{
    namespace gui
    {
        class Window : public CompositeWidget
        {
        protected:
            bool Dragging;

        public:
            Window(
                Widget *parent,
                common::int32_t x,
                common::int32_t y,
                common::int32_t w,
                common::int32_t h,
                common::int32_t r,
                common::int32_t g,
                common::int32_t b);
            ~Window();

            virtual void OnMouseDown(
                common::int32_t x,
                common::int32_t y,
                common::uint8_t button);

            virtual void OnMouseUp(
                common::int32_t x,
                common::int32_t y,
                common::uint8_t button);

            virtual void OnMouseMove(
                common::int32_t oldX,
                common::int32_t oldY,
                common::int32_t newX,
                common::int32_t newY);
        };
    }
}

#endif