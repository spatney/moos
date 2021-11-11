#ifndef __MOOS__DRIVERS__VGA_H
#define __MOOS__DRIVERS__VGA_H

#include <common/types.h>
#include <hardware/port.h>

namespace moos
{
    namespace drivers
    {
        class VideoGraphicsArray
        {
        private:
            hardware::Port8Bit miscPort;
            hardware::Port8Bit crtcIndexPort;
            hardware::Port8Bit crtcDataPort;
            hardware::Port8Bit sequencerIndexPort;
            hardware::Port8Bit sequencerDataport;
            hardware::Port8Bit graphicsControllerIndexPort;
            hardware::Port8Bit graphicsControllerDataPort;
            hardware::Port8Bit attributeControllerIndexPort;
            hardware::Port8Bit attributeControllerReadPort;
            hardware::Port8Bit attributeControllerWritePort;
            hardware::Port8Bit attributeControllerResetPort;

            void WriteRegisters(common::uint8_t *registers);
            common::uint8_t *GetFrameBufferSergment();

            common::uint8_t buffer [320*200];
            common::uint8_t lastBuffer [320*200];

            virtual common::uint8_t GetColorIndex(
                common::uint8_t r,
                common::uint8_t g,
                common::uint8_t b);

        public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();

            virtual bool setMode(
                common::uint32_t width,
                common::uint32_t height,
                common::uint32_t colorDepth);

            virtual bool SupportsMode(
                common::uint32_t width,
                common::uint32_t height,
                common::uint32_t colorDepth);

            virtual void PutPixel(
                common::int32_t x,
                common::int32_t y,
                common::uint8_t r,
                common::uint8_t g,
                common::uint8_t b);

            virtual void PutPixel(
                common::int32_t x,
                common::int32_t y,
                common::uint8_t colorIndex);

            virtual void Finalize();
        };
    }
}

#endif
