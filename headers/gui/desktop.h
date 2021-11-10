#ifndef __MOOS__GUI__DESKTOP_H
#define __MOOS__GUI__DESKTOP_H

#include <common/types.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <gui/widget.h>

namespace moos
{
    namespace gui
    {
        class Desktop : public CompositeWidget, public drivers::MouseEventHandler
        {
        protected:
            common::int32_t MouseX;
            common::int32_t MouseY;
            common::int32_t LastMouseX;
            common::int32_t LastMouseY;

        public:
            Desktop(
                common::int32_t w,
                common::int32_t h,
                common::int32_t r,
                common::int32_t g,
                common::int32_t b);

            ~Desktop();
            void Draw(gui::GraphicsContext *gc);

            void OnMouseDown(
                common::int32_t x,
                common::int32_t y,
                common::uint8_t button);

            void OnMouseUp(
                common::int32_t x,
                common::int32_t y,
                common::uint8_t button);

            void OnMouseMove(
                common::int32_t xOffset,
                common::int32_t yOffset);
        };
    }
}
#endif