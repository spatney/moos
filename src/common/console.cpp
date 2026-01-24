#include <common/console.h>
#include <stdarg.h>
#include <hardware/video.h>
#include <hardware/port.h>

using namespace moos::common;
using namespace moos::hardware;

// Console state
static int cursorX = 0;
static int cursorY = 0;
static uint8_t currentColor = 0x07; // Light gray on black

// VGA ports
static Port8Bit vgaIndexPort(0x3D4);
static Port8Bit vgaDataPort(0x3D5);

void Console::Init()
{
    currentColor = 0x07;
    
    // Enable VGA text cursor (underline style, blinking)
    vgaIndexPort.Write(0x0A);  // Cursor start register
    vgaDataPort.Write(0x0E);   // Start at scanline 14 (underline)
    vgaIndexPort.Write(0x0B);  // Cursor end register
    vgaDataPort.Write(0x0F);   // End at scanline 15
    
    Clear();
}

void Console::Clear()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            VideoMemory[y * WIDTH + x] = (currentColor << 8) | ' ';
        }
    }
    cursorX = 0;
    cursorY = 0;
    
    // Re-enable cursor after clear
    vgaIndexPort.Write(0x0A);
    vgaDataPort.Write(0x0E);
    vgaIndexPort.Write(0x0B);
    vgaDataPort.Write(0x0F);
    
    UpdateHardwareCursor();
}

void Console::SetCursor(int x, int y)
{
    if (x >= 0 && x < WIDTH) cursorX = x;
    if (y >= 0 && y < HEIGHT) cursorY = y;
    UpdateHardwareCursor();
}

int Console::GetCursorX() { return cursorX; }
int Console::GetCursorY() { return cursorY; }

void Console::HideCursor()
{
    vgaIndexPort.Write(0x0A);
    vgaDataPort.Write(0x20);  // Bit 5 disables cursor
}

void Console::ShowCursor()
{
    vgaIndexPort.Write(0x0A);
    vgaDataPort.Write(0x0E);  // Scanline 14
    vgaIndexPort.Write(0x0B);
    vgaDataPort.Write(0x0F);  // Scanline 15
    UpdateHardwareCursor();
}

uint8_t Console::SetColor(uint8_t color)
{
    uint8_t old = currentColor;
    currentColor = color;
    return old;
}

uint8_t Console::GetColor() { return currentColor; }

void Console::PutCharAt(char c, int x, int y, uint8_t color)
{
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
    {
        VideoMemory[y * WIDTH + x] = (color << 8) | (uint8_t)c;
    }
}

void Console::ScrollUp()
{
    // Move all lines up by one
    for (int y = 0; y < HEIGHT - 1; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            VideoMemory[y * WIDTH + x] = VideoMemory[(y + 1) * WIDTH + x];
        }
    }
    // Clear bottom line
    for (int x = 0; x < WIDTH; x++)
    {
        VideoMemory[(HEIGHT - 1) * WIDTH + x] = (currentColor << 8) | ' ';
    }
}

void Console::PutChar(char c)
{
    switch (c)
    {
    case '\n':
        cursorX = 0;
        cursorY++;
        break;
    case '\r':
        cursorX = 0;
        break;
    case '\t':
        cursorX = (cursorX + 4) & ~3; // Align to 4
        break;
    case '\b':
        if (cursorX > 0) cursorX--;
        break;
    default:
        PutCharAt(c, cursorX, cursorY, currentColor);
        cursorX++;
        break;
    }
    
    // Handle line wrap
    if (cursorX >= WIDTH)
    {
        cursorX = 0;
        cursorY++;
    }
    
    // Handle scroll
    if (cursorY >= HEIGHT)
    {
        ScrollUp();
        cursorY = HEIGHT - 1;
    }
    
    UpdateHardwareCursor();
}

void Console::PutCP437(uint8_t c)
{
    PutCharAt((char)c, cursorX, cursorY, currentColor);
    cursorX++;
    
    if (cursorX >= WIDTH)
    {
        cursorX = 0;
        cursorY++;
    }
    
    if (cursorY >= HEIGHT)
    {
        ScrollUp();
        cursorY = HEIGHT - 1;
    }
    
    UpdateHardwareCursor();
}

// UTF-8 to CP437 translation for common box-drawing characters
static uint8_t Utf8ToCp437(const char *&str)
{
    uint8_t c = (uint8_t)*str;
    
    // Single byte ASCII - pass through
    if (c < 0x80)
    {
        str++;
        return c;
    }
    
    // 3-byte UTF-8 sequences (most box drawing chars)
    if ((c & 0xF0) == 0xE0 && str[1] && str[2])
    {
        uint32_t codepoint = ((c & 0x0F) << 12) | 
                            ((str[1] & 0x3F) << 6) | 
                            (str[2] & 0x3F);
        str += 3;
        
        // Box drawing characters (U+2500-U+257F)
        switch (codepoint)
        {
            case 0x2500: return 0xC4; // ─ horizontal
            case 0x2502: return 0xB3; // │ vertical
            case 0x250C: return 0xDA; // ┌ top-left
            case 0x2510: return 0xBF; // ┐ top-right
            case 0x2514: return 0xC0; // └ bottom-left
            case 0x2518: return 0xD9; // ┘ bottom-right
            case 0x251C: return 0xC3; // ├ left-tee
            case 0x2524: return 0xB4; // ┤ right-tee
            case 0x252C: return 0xC2; // ┬ top-tee
            case 0x2534: return 0xC1; // ┴ bottom-tee
            case 0x253C: return 0xC5; // ┼ cross
            case 0x2550: return 0xCD; // ═ double horizontal
            case 0x2551: return 0xBA; // ║ double vertical
            case 0x2554: return 0xC9; // ╔ double top-left
            case 0x2557: return 0xBB; // ╗ double top-right
            case 0x255A: return 0xC8; // ╚ double bottom-left
            case 0x255D: return 0xBC; // ╝ double bottom-right
            case 0x2560: return 0xCC; // ╠ double left-tee
            case 0x2563: return 0xB9; // ╣ double right-tee
            case 0x2566: return 0xCB; // ╦ double top-tee
            case 0x2569: return 0xCA; // ╩ double bottom-tee
            case 0x256C: return 0xCE; // ╬ double cross
            case 0x256D: return 0xDA; // ╭ rounded top-left (use regular)
            case 0x256E: return 0xBF; // ╮ rounded top-right
            case 0x256F: return 0xD9; // ╯ rounded bottom-right
            case 0x2570: return 0xC0; // ╰ rounded bottom-left
            case 0x2580: return 0xDF; // ▀ upper half block
            case 0x2584: return 0xDC; // ▄ lower half block
            case 0x2588: return 0xDB; // █ full block
            case 0x258C: return 0xDD; // ▌ left half block
            case 0x2590: return 0xDE; // ▐ right half block
            case 0x2591: return 0xB0; // ░ light shade
            case 0x2592: return 0xB1; // ▒ medium shade
            case 0x2593: return 0xB2; // ▓ dark shade
            case 0x25A0: return 0xFE; // ■ black square
            case 0x25B2: return 0x1E; // ▲ triangle up
            case 0x25BA: return 0x10; // ► triangle right
            case 0x25BC: return 0x1F; // ▼ triangle down
            case 0x25C4: return 0x11; // ◄ triangle left
            case 0x25CF: return 0x09; // ● black circle
            case 0x25CB: return 0x09; // ○ white circle (use same)
            case 0x2018: return 0x60; // ' left single quote
            case 0x2019: return 0x27; // ' right single quote
            case 0x201C: return 0x22; // " left double quote
            case 0x201D: return 0x22; // " right double quote
            case 0x2022: return 0x07; // • bullet
            case 0x2026: return '.';  // … ellipsis (simplified)
            case 0x2014: return 0xC4; // — em dash
            case 0x2013: return 0xC4; // – en dash
            case 0x2594: return 0xC4; // ▔ upper one eighth block (use horizontal line)
            case 0x2598: return 0xDC; // ▘ quadrant upper left (use lower block)
            case 0x259D: return 0xDC; // ▝ quadrant upper right
            default: return '?';
        }
    }
    
    // 2-byte UTF-8 sequences
    if ((c & 0xE0) == 0xC0 && str[1])
    {
        uint32_t codepoint = ((c & 0x1F) << 6) | (str[1] & 0x3F);
        str += 2;
        
        // Common Latin-1 supplement chars
        if (codepoint >= 0x80 && codepoint <= 0xFF)
            return (uint8_t)codepoint; // Direct mapping works for many
        
        return '?';
    }
    
    // Unknown - skip byte
    str++;
    return '?';
}

void Console::Print(const char *str)
{
    while (*str)
    {
        uint8_t c = Utf8ToCp437(str);
        PutChar((char)c);
    }
}

void Console::Printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    char numBuf[32];
    
    while (*format)
    {
        if (*format == '%' && *(format + 1))
        {
            format++;
            switch (*format)
            {
            case 'd':
            case 'i':
                Print(IntToStr(va_arg(args, int32_t), 10, numBuf));
                break;
            case 'x':
            case 'X':
                Print(IntToStr(va_arg(args, int32_t), 16, numBuf));
                break;
            case 's':
                Print(va_arg(args, char*));
                break;
            case 'c':
                PutChar((char)va_arg(args, int));
                break;
            case '%':
                PutChar('%');
                break;
            case '0':
                // Handle %02d style formatting
                if (*(format + 1) == '2' && *(format + 2) == 'd')
                {
                    int32_t val = va_arg(args, int32_t);
                    if (val >= 0 && val < 10) PutChar('0');
                    Print(IntToStr(val, 10, numBuf));
                    format += 2;
                }
                break;
            default:
                PutChar('%');
                PutChar(*format);
                break;
            }
        }
        else
        {
            PutChar(*format);
        }
        format++;
    }
    
    va_end(args);
}

void Console::Backspace()
{
    if (cursorX > 0)
    {
        cursorX--;
        PutCharAt(' ', cursorX, cursorY, currentColor);
        UpdateHardwareCursor();
    }
    else if (cursorY > 0)
    {
        cursorY--;
        cursorX = WIDTH - 1;
        PutCharAt(' ', cursorX, cursorY, currentColor);
        UpdateHardwareCursor();
    }
}

void Console::Sleep(int32_t milliseconds)
{
    uint32_t ticksToWait = (milliseconds * 18) / 1000; // ~18.2 ticks per second
    if (ticksToWait == 0) ticksToWait = 1;
    uint32_t start = ticks;
    while ((ticks - start) < ticksToWait)
        ;
}

void Console::UpdateHardwareCursor()
{
    uint16_t pos = cursorY * WIDTH + cursorX;
    
    vgaIndexPort.Write(0x0F);
    vgaDataPort.Write((uint8_t)(pos & 0xFF));
    vgaIndexPort.Write(0x0E);
    vgaDataPort.Write((uint8_t)((pos >> 8) & 0xFF));
}

char *Console::IntToStr(int32_t val, int base, char *buf)
{
    static const char digits[] = "0123456789ABCDEF";
    char *p = buf + 30;
    *p = '\0';
    
    bool negative = false;
    uint32_t uval;
    
    if (base == 10 && val < 0)
    {
        negative = true;
        uval = -val;
    }
    else
    {
        uval = (uint32_t)val;
    }
    
    do
    {
        *--p = digits[uval % base];
        uval /= base;
    } while (uval);
    
    if (negative)
    {
        *--p = '-';
    }
    
    return p;
}