#ifndef __MOOS__COMMON__CONSOLE_H
#define __MOOS__COMMON__CONSOLE_H

#include <common/types.h>

namespace moos
{
    namespace common
    {
        class Console
        {
        public:
            static const int WIDTH = 80;
            static const int HEIGHT = 25;
            
            // System tick counter (incremented by timer interrupt)
            inline static uint32_t ticks = 0;
            
            // Initialize console
            static void Init();
            
            // Clear screen
            static void Clear();
            
            // Cursor control
            static void SetCursor(int x, int y);
            static int GetCursorX();
            static int GetCursorY();
            
            // Color control (returns previous color)
            static uint8_t SetColor(uint8_t color);
            static uint8_t GetColor();
            
            // Output
            static void PutChar(char c);
            static void PutCP437(uint8_t c);  // Direct CP437 character
            static void Print(const char *str);
            static void Printf(const char *format, ...);
            
            // Input helpers
            static void Backspace();
            
            // Scrolling
            static void ScrollUp();
            
            // Timing
            static void Sleep(int32_t milliseconds);
            
        private:
            static void UpdateHardwareCursor();
            static void PutCharAt(char c, int x, int y, uint8_t color);
            static char *IntToStr(int32_t val, int base, char *buf);
        };
    }
}

#endif