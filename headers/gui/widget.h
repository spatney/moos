#ifndef __MOOS__GUI__Widget_H
#define __MOOS__GUI__Widget_H

#include <common/types.h>
#include <gui/graphics.h>

namespace moos
{
    namespace gui
    {
        class Widget
        {
        protected:
            Widget *parent;
            common::int32_t x;
            common::int32_t y;
            common::int32_t w;
            common::int32_t h;

            common::uint8_t r;
            common::uint8_t g;
            common::uint8_t b;

            bool focusable;

        public:
            Widget(
                Widget *parent,
                common::int32_t x,
                common::int32_t y,
                common::int32_t w,
                common::int32_t h,
                common::int32_t r,
                common::int32_t g,
                common::int32_t b);
            ~Widget();

            virtual void Draw(GraphicsContext *gc);
            virtual void GetFocus(Widget *widget);

            virtual void ModelToScreen(
                common::int32_t &x,
                common::int32_t &y);

            virtual void OnMouseDown(
                common::int32_t x,
                common::int32_t y);

            virtual void OnMouseUp(
                common::int32_t x,
                common::int32_t y);

            virtual void OnMouseMove(
                common::int32_t oldX,
                common::int32_t oldY,
                common::int32_t newX,
                common::int32_t newY);

            virtual void OnKeyDown(char *str);

            virtual void OnKeyUp(char *str);

            virtual bool ContainsCoordinate(
                common::int32_t x,
                common::int32_t y);
        };

        class CompositeWidget : public Widget
        {
        private:
            Widget *children[100];
            common::int32_t numChildren;
            Widget *focussedChild;

        public:
            CompositeWidget(
                Widget *parent,
                common::int32_t x,
                common::int32_t y,
                common::int32_t w,
                common::int32_t h,
                common::int32_t r,
                common::int32_t g,
                common::int32_t b);
            ~CompositeWidget();

            virtual void Draw(GraphicsContext *gc);
            virtual void GetFocus(Widget *widget);

            virtual void OnMouseDown(
                common::int32_t x,
                common::int32_t y);

            virtual void OnMouseUp(
                common::int32_t x,
                common::int32_t y);

            virtual void OnMouseMove(
                common::int32_t oldX,
                common::int32_t oldY,
                common::int32_t newX,
                common::int32_t newY);

            virtual void OnKeyUp(char *str);

            virtual void OnKeyDown(char *str);
        };
    }
}

#endif