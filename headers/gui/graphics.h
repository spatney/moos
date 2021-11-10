#ifndef __MOOS__GUI__GRAPHICS_H
#define __MOOS__GUI__GRAPHICS_H

#include <drivers/vga.h>
#include <common/types.h>

namespace moos
{
    namespace gui
    {
        class GraphicsContext
        {
        private:
            drivers::VideoGraphicsArray vga;

            common::int32_t abs(common::int32_t n);
        public:
            GraphicsContext();
            ~GraphicsContext();

            virtual void FillRectangle(
                common::int32_t x,
                common::int32_t y,
                common::int32_t w,
                common::int32_t h,
                common::int8_t r,
                common::int8_t g,
                common::int8_t b);

            virtual void DrawLine(
                common::int32_t x0,
                common::int32_t y0,
                common::int32_t x1,
                common::int32_t y1,
                common::int8_t r,
                common::int8_t g,
                common::int8_t b);
        };
    }
}

#endif