#include <gui/editor.h>
#include <hardware/video.h>
#include <common/console.h>
#include <common/strings.h>
#include <core/memory.h>
#include <drivers/disk.h>
#include <file_system/msdos_partition.h>

using namespace moos;
using namespace moos::gui;
using namespace moos::common;
using namespace moos::hardware;
using namespace moos::drivers;
using namespace moos::core;
using namespace moos::file_system;

Editor::Editor(KeyboardDriver *keyboardDriver, KeyboardEventHandler *previousHandler)
    : keyboardDriver(keyboardDriver),
      previousHandler(previousHandler),
      buffer(new int8_t[8192]),
      filename(new int8_t[32]),
      bufferCapacity(8192)
{
    bufferSize = 0;
    cursorX = 0;
    cursorY = 0;
    scrollOffset = 0;
    isShiftDown = false;
    modified = false;
    running = true;
}

Editor::~Editor()
{
    delete[] buffer;
    delete[] filename;
}

void Editor::Open(const int8_t *name, int8_t *contents)
{
    // Copy filename
    int i = 0;
    while (name[i] != '\0' && i < 31)
    {
        filename[i] = name[i];
        i++;
    }
    filename[i] = '\0';
    
    // Copy contents to buffer
    bufferSize = 0;
    if (contents != nullptr)
    {
        while (contents[bufferSize] != '\0' && bufferSize < bufferCapacity - 1)
        {
            buffer[bufferSize] = contents[bufferSize];
            bufferSize++;
        }
    }
    buffer[bufferSize] = '\0';
    
    cursorX = 0;
    cursorY = 0;
    scrollOffset = 0;
    modified = false;
    
    draw();
}

int32_t Editor::getLineCount()
{
    int32_t count = 1;
    for (size_t i = 0; i < bufferSize; i++)
    {
        if (buffer[i] == '\n')
            count++;
    }
    return count;
}

size_t Editor::getLineStart(int32_t line)
{
    if (line == 0)
        return 0;
    
    int32_t currentLine = 0;
    for (size_t i = 0; i < bufferSize; i++)
    {
        if (buffer[i] == '\n')
        {
            currentLine++;
            if (currentLine == line)
                return i + 1;
        }
    }
    return bufferSize;
}

size_t Editor::getLineLength(int32_t line)
{
    size_t start = getLineStart(line);
    size_t len = 0;
    while (start + len < bufferSize && buffer[start + len] != '\n')
    {
        len++;
    }
    return len;
}

size_t Editor::getCursorPosition()
{
    size_t pos = getLineStart(cursorY);
    pos += cursorX;
    if (pos > bufferSize)
        pos = bufferSize;
    return pos;
}

void Editor::draw()
{
    // Draw directly to video memory for efficiency
    uint16_t *video = VideoMemory;
    
    // Draw header (line 0)
    const uint16_t headerAttr = 0x7000; // Inverted (black on white)
    const uint16_t textAttr = 0x0700;   // Normal (white on black)
    
    // Clear and draw header
    const char *title = "  MoOS nano - ";
    int col = 0;
    for (int i = 0; title[i] != '\0' && col < SCREEN_WIDTH; i++, col++)
        video[col] = headerAttr | title[i];
    for (int i = 0; filename[i] != '\0' && col < SCREEN_WIDTH; i++, col++)
        video[col] = headerAttr | filename[i];
    if (modified)
    {
        const char *mod = " (modified)";
        for (int i = 0; mod[i] != '\0' && col < SCREEN_WIDTH; i++, col++)
            video[col] = headerAttr | mod[i];
    }
    while (col < SCREEN_WIDTH)
        video[col++] = headerAttr | ' ';
    
    // Draw content (lines 1 to EDIT_HEIGHT)
    int32_t line = scrollOffset;
    for (int32_t y = 1; y <= EDIT_HEIGHT && line < getLineCount(); y++, line++)
    {
        size_t start = getLineStart(line);
        size_t len = getLineLength(line);
        
        col = 0;
        for (size_t x = 0; x < len && col < SCREEN_WIDTH; x++, col++)
        {
            video[y * 80 + col] = textAttr | buffer[start + x];
        }
        // Clear rest of line
        while (col < SCREEN_WIDTH)
            video[y * 80 + col++] = textAttr | ' ';
    }
    
    // Clear remaining content lines
    for (int32_t y = 1 + getLineCount() - scrollOffset; y <= EDIT_HEIGHT; y++)
    {
        for (int col = 0; col < SCREEN_WIDTH; col++)
            video[y * 80 + col] = textAttr | ' ';
    }
    
    // Draw help bar (line 23)
    const uint16_t helpAttr = 0x7000;
    const char *help = "^X Exit  ^O Save                                                                ";
    for (int i = 0; i < SCREEN_WIDTH; i++)
        video[23 * 80 + i] = helpAttr | help[i];
    
    // Draw status bar (line 24)
    char status[81];
    int slen = 0;
    status[slen++] = ' ';
    status[slen++] = ' ';
    status[slen++] = 'L';
    status[slen++] = 'i';
    status[slen++] = 'n';
    status[slen++] = 'e';
    status[slen++] = ' ';
    // Simple int to string for line number
    int32_t lineNum = cursorY + 1;
    char numBuf[12];
    int numLen = 0;
    do {
        numBuf[numLen++] = '0' + (lineNum % 10);
        lineNum /= 10;
    } while (lineNum > 0);
    for (int i = numLen - 1; i >= 0; i--)
        status[slen++] = numBuf[i];
    status[slen++] = ',';
    status[slen++] = ' ';
    status[slen++] = 'C';
    status[slen++] = 'o';
    status[slen++] = 'l';
    status[slen++] = ' ';
    int32_t colNum = cursorX + 1;
    numLen = 0;
    do {
        numBuf[numLen++] = '0' + (colNum % 10);
        colNum /= 10;
    } while (colNum > 0);
    for (int i = numLen - 1; i >= 0; i--)
        status[slen++] = numBuf[i];
    while (slen < SCREEN_WIDTH)
        status[slen++] = ' ';
    
    for (int i = 0; i < SCREEN_WIDTH; i++)
        video[24 * 80 + i] = helpAttr | status[i];
    
    // Highlight cursor position
    int32_t screenY = 1 + (cursorY - scrollOffset);
    int32_t screenX = cursorX;
    if (screenY >= 1 && screenY <= EDIT_HEIGHT && screenX < SCREEN_WIDTH)
    {
        video[screenY * 80 + screenX] = (video[screenY * 80 + screenX] & 0x00FF) | 0xF000;
    }
}

void Editor::moveCursor(int32_t dx, int32_t dy)
{
    int32_t lineCount = getLineCount();
    
    // Vertical movement
    cursorY += dy;
    if (cursorY < 0)
        cursorY = 0;
    if (cursorY >= lineCount)
        cursorY = lineCount - 1;
    
    // Horizontal movement
    int32_t lineLen = getLineLength(cursorY);
    cursorX += dx;
    if (cursorX < 0)
    {
        if (cursorY > 0)
        {
            cursorY--;
            cursorX = getLineLength(cursorY);
        }
        else
        {
            cursorX = 0;
        }
    }
    if (cursorX > lineLen)
    {
        if (cursorY < lineCount - 1)
        {
            cursorY++;
            cursorX = 0;
        }
        else
        {
            cursorX = lineLen;
        }
    }
    
    // Adjust scroll
    if (cursorY < scrollOffset)
        scrollOffset = cursorY;
    if (cursorY >= scrollOffset + EDIT_HEIGHT)
        scrollOffset = cursorY - EDIT_HEIGHT + 1;
    
    draw();
}

void Editor::insertChar(int8_t c)
{
    if (bufferSize >= bufferCapacity - 1)
        return;
    
    size_t pos = getCursorPosition();
    
    // Shift everything right
    for (size_t i = bufferSize; i > pos; i--)
    {
        buffer[i] = buffer[i - 1];
    }
    
    buffer[pos] = c;
    bufferSize++;
    buffer[bufferSize] = '\0';
    
    cursorX++;
    modified = true;
    draw();
}

void Editor::deleteChar()
{
    size_t pos = getCursorPosition();
    
    if (pos == 0)
        return;
    
    // Check if we're deleting a newline
    if (buffer[pos - 1] == '\n')
    {
        cursorY--;
        cursorX = getLineLength(cursorY);
    }
    else
    {
        cursorX--;
    }
    
    // Shift everything left
    for (size_t i = pos - 1; i < bufferSize; i++)
    {
        buffer[i] = buffer[i + 1];
    }
    
    bufferSize--;
    modified = true;
    draw();
}

void Editor::newLine()
{
    if (bufferSize >= bufferCapacity - 1)
        return;
    
    size_t pos = getCursorPosition();
    
    // Shift everything right
    for (size_t i = bufferSize; i > pos; i--)
    {
        buffer[i] = buffer[i - 1];
    }
    
    buffer[pos] = '\n';
    bufferSize++;
    buffer[bufferSize] = '\0';
    
    cursorY++;
    cursorX = 0;
    modified = true;
    draw();
}

void Editor::save()
{
    auto disk = DiskManager::GetDisk();
    
    if (disk == 0)
    {
        for (int i = 0; i < SCREEN_WIDTH; i++)
            VideoMemory[80 * 24 + i] = 0x4F00 | ' ';
        const char *msg = "  [ Error: No disk found ]";
        for (int i = 0; msg[i] != '\0' && i < SCREEN_WIDTH; i++)
            VideoMemory[80 * 24 + i] = 0x4F00 | msg[i];
        return;
    }
    
    bool success = PartitionTable::WriteFileContents(disk, filename, buffer, bufferSize);
    
    // Show status message at bottom
    for (int i = 0; i < SCREEN_WIDTH; i++)
        VideoMemory[80 * 24 + i] = 0x4F00 | ' ';
    
    if (success)
    {
        const char *msg = "  [ File saved successfully ]";
        for (int i = 0; msg[i] != '\0' && i < SCREEN_WIDTH; i++)
            VideoMemory[80 * 24 + i] = 0x2F00 | msg[i]; // Green background
        modified = false;
    }
    else
    {
        const char *msg = "  [ Error: Could not save file ]";
        for (int i = 0; msg[i] != '\0' && i < SCREEN_WIDTH; i++)
            VideoMemory[80 * 24 + i] = 0x4F00 | msg[i]; // Red background
    }
}

void Editor::exit()
{
    running = false;
    Console::Clear();
    keyboardDriver->SetEventHandler(previousHandler);
}

uint8_t Editor::keyToChar(Key key)
{
    if (key <= Key::Z && key >= Key::A)
    {
        return (key - Key::A) + (isShiftDown ? 65 : 97);
    }
    if (key <= Key::Num9 && key >= Key::Num0 && !isShiftDown)
    {
        return (key - Key::Num0) + 48;
    }
    if (key == Key::Space)
    {
        return ' ';
    }

    switch (key)
    {
    case Key::Num0: return ')';
    case Key::Num1: return '!';
    case Key::Num2: return '@';
    case Key::Num3: return '#';
    case Key::Num4: return '$';
    case Key::Num5: return '%';
    case Key::Num6: return '^';
    case Key::Num7: return '&';
    case Key::Num8: return '*';
    case Key::Num9: return '(';
    case Key::Period: return isShiftDown ? '>' : '.';
    case Key::Comma: return isShiftDown ? '<' : ',';
    case Key::Slash: return isShiftDown ? '?' : '/';
    case Key::Backslash: return isShiftDown ? '|' : '\\';
    case Key::Hyphen: return isShiftDown ? '_' : '-';
    case Key::Equal: return isShiftDown ? '+' : '=';
    case Key::Semicolon: return isShiftDown ? ':' : ';';
    case Key::Quote: return isShiftDown ? '"' : '\'';
    case Key::LBracket: return isShiftDown ? '{' : '[';
    case Key::RBracket: return isShiftDown ? '}' : ']';
    case Key::Tilde: return isShiftDown ? '~' : '`';
    default: return '\0';
    }
}

void Editor::OnKeyUp(Key key)
{
    switch (key)
    {
    case Key::RShift:
    case Key::LShift:
        isShiftDown = false;
        break;
    case Key::LControl:
    case Key::RControl:
        break;
    default:
        break;
    }
}

void Editor::OnKeyDown(Key key)
{
    switch (key)
    {
    case Key::RShift:
    case Key::LShift:
        isShiftDown = true;
        break;
        
    case Key::Up:
        moveCursor(0, -1);
        break;
    case Key::Down:
        moveCursor(0, 1);
        break;
    case Key::Left:
        moveCursor(-1, 0);
        break;
    case Key::Right:
        moveCursor(1, 0);
        break;
        
    case Key::Backspace:
        deleteChar();
        break;
        
    case Key::Enter:
        newLine();
        break;
        
    case Key::X:
        if (isShiftDown) // Ctrl+X approximation (using Shift+X)
        {
            exit();
        }
        else
        {
            insertChar('x');
        }
        break;
        
    case Key::O:
        if (isShiftDown) // Ctrl+O approximation (using Shift+O)
        {
            save();
        }
        else
        {
            insertChar('o');
        }
        break;
        
    case Key::Escape:
        exit();
        break;
        
    default:
        auto c = keyToChar(key);
        if (c != '\0')
        {
            insertChar(c);
        }
        break;
    }
}
