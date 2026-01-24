#include <gui/terminal.h>
#include <gui/editor.h>
#include <hardware/video.h>
#include <common/console.h>
#include <common/strings.h>
#include <core/memory.h>
#include <hardware/pci.h>
#include <hardware/interrupts.h>
#include <drivers/disk.h>
#include <file_system/msdos_partition.h>
#include <ostest.h>

using namespace moos;
using namespace moos::gui;
using namespace moos::common;
using namespace moos::hardware;
using namespace moos::drivers;
using namespace moos::core;
using namespace moos::file_system;

extern bool kernel_running;

Terminal::Terminal(KeyboardDriver *keyboardDriver)
    : history(new LinkedList()),
      historyIt(history->end()),
      buffer(new int8_t[2000]),
      keyboardDriver(keyboardDriver),
      activeEditor(nullptr)
{
    x = 40;
    y = 12;
    isShiftDown = false;
    isCapsLockOn = false;
    isCtrlDown = false;
    bufferCount = 0;
    cursorPos = 0;
    promptX = 0;
    promptY = 0;
}

Terminal::~Terminal()
{
}

void Terminal::OnMouseDown(uint8_t button)
{
    invertVideoMemoryAt(x, y);
}

void Terminal::OnMouseUp(uint8_t button)
{
    invertVideoMemoryAt(x, y);
}

void Terminal::OnMouseMove(int32_t xOffset, int32_t yOffset)
{
    xOffset /= 4;
    yOffset /= 4;

    invertVideoMemoryAt(x, y);
    x += xOffset;
    y += yOffset;

    if (x < 0)
        x = 0;
    if (x >= 80)
        x = 79;
    if (y < 0)
        y = 0;
    if (y >= 25)
        y = 24;

    invertVideoMemoryAt(x, y);
}
void Terminal::invertVideoMemoryAt(int8_t x, int8_t y)
{
    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
}

LinkedList *Terminal::tokenizeBuffer()
{
    auto list = new LinkedList();
    auto tempBuffer = buffer;

    while (*tempBuffer != '\0')
    {
        if (*tempBuffer != ' ')
        {
            list->AddLast(tempBuffer);

            while (*tempBuffer != ' ' && *tempBuffer != '\0')
            {
                ++tempBuffer;
            }

            if (*tempBuffer != '\0')
            {
                *tempBuffer++ = '\0';
            }
        }
        else
        {
            *tempBuffer++;
        }
    }

    return list;
}

uint8_t Terminal::keyToChar(Key key)
{
    if (key <= Key::Z && key >= Key::A)
    {
        return (key - Key::A) + (isShiftDown ^ isCapsLockOn ? 65 : 97);
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
    case Key::Num0:
        return ')';
    case Key::Num1:
        return '!';
    case Key::Num2:
        return '@';
    case Key::Num3:
        return '#';
    case Key::Num4:
        return '$';
    case Key::Num5:
        return '%';
    case Key::Num6:
        return '^';
    case Key::Num7:
        return '&';
    case Key::Num8:
        return '*';
    case Key::Num9:
        return '(';
    case Key::Period:
        return isShiftDown ? '>' : '.';
    case Key::Comma:
        return isShiftDown ? '<' : ',';
    case Key::Slash:
        return isShiftDown ? '?' : '/';
    case Key::Backslash:
        return isShiftDown ? '|' : '\\';
    case Key::Tab:
        return '\t';
    case Key::Enter:
        return '\n';
    case Key::Hyphen:
        return isShiftDown ? '_' : '-';
    case Key::Equal:
        return isShiftDown ? '+' : '=';
    default:
        return '\0';
    }
}

void Terminal::OnKeyDown(Key key)
{
    // Clean up editor if it exited
    if (activeEditor != nullptr && !activeEditor->IsRunning())
    {
        delete activeEditor;
        activeEditor = nullptr;
        drawPrompt();
    }
    
    switch (key)
    {
    case Key::Up:
    case Key::Down:
    {
        if (key == Key::Down)
        {
            if (historyIt == history->begin())
                historyIt = history->last();
            else
            {
                --historyIt;
                if (historyIt == history->end())
                    historyIt = history->begin();
            }
        }
        else
        {
            if (historyIt == history->end())
                historyIt = history->begin();
            else
            {
                ++historyIt;
                if (historyIt == history->end())
                    historyIt = history->begin();
            }
        }

        if (historyIt != history->end())
        {
            clearInput();
            bufferCount = copyToBuffer((int8_t *)*historyIt);
            cursorPos = bufferCount;
            redrawInput();
        }
        break;
    }
    
    case Key::Left:
        if (isCtrlDown)
        {
            // Move to previous word
            while (cursorPos > 0 && buffer[cursorPos - 1] == ' ')
                cursorPos--;
            while (cursorPos > 0 && buffer[cursorPos - 1] != ' ')
                cursorPos--;
        }
        else
            moveCursorLeft();
        redrawInput();
        break;
        
    case Key::Right:
        if (isCtrlDown)
        {
            // Move to next word
            while (cursorPos < bufferCount && buffer[cursorPos] != ' ')
                cursorPos++;
            while (cursorPos < bufferCount && buffer[cursorPos] == ' ')
                cursorPos++;
        }
        else
            moveCursorRight();
        redrawInput();
        break;
        
    case Key::Home:
        moveCursorToStart();
        redrawInput();
        break;
        
    case Key::End:
        moveCursorToEnd();
        redrawInput();
        break;
    
    case Key::RShift:
    case Key::LShift:
        isShiftDown = true;
        break;
        
    case Key::LControl:
    case Key::RControl:
        isCtrlDown = true;
        break;
        
    case Key::CapsLock:
        isCapsLockOn = !isCapsLockOn;
        break;
        
    case Key::Backspace:
        if (cursorPos > 0)
        {
            deleteCharAtCursor();
            redrawInput();
        }
        break;
        
    case Key::Delete:
        if (cursorPos < bufferCount)
        {
            cursorPos++;
            deleteCharAtCursor();
            redrawInput();
        }
        break;
        
    case Key::Tab:
        // Insert 4 spaces
        for (int i = 0; i < 4; i++)
            insertCharAtCursor(' ');
        redrawInput();
        break;
        
    case Key::Enter:
    {
        Console::Print("\n");

        buffer[bufferCount] = '\0';

        if (StringUtil::strcmp((const int8_t *)history->PeekFirst(), buffer))
        {
            history->AddFirst(copyBuffer());
        }
        historyIt = history->end();

        auto tokens = tokenizeBuffer();

        if (!StringUtil::strcmp("clear", (const int8_t *)tokens->PeekFirst()))
        {
            Console::Clear();
        }
        else if (!StringUtil::strcmp("help", (const int8_t *)tokens->PeekFirst()))
        {
            auto color = Console::SetColor(0xE);
            Console::Printf("Available commands:\n");
            Console::SetColor(color);
            Console::Printf("  help     - show this help message\n");
            Console::Printf("  clear    - clear the screen\n");
            Console::Printf("  echo     - print text to console\n");
            Console::Printf("  ls       - list files on disk\n");
            Console::Printf("  cat      - display file contents\n");
            Console::Printf("  nano     - text editor (Esc to exit)\n");
            Console::Printf("  pwd      - print working directory\n");
            Console::Printf("  uname    - show kernel version\n");
            Console::Printf("  uptime   - show system uptime\n");
            Console::Printf("  free     - show memory usage\n");
            Console::Printf("  lspci    - list PCI devices\n");
            Console::Printf("  reboot   - reboot the system\n");
            Console::Printf("  halt     - halt the system\n");
        }
        else if (!StringUtil::strcmp("echo", (const int8_t *)tokens->PeekFirst()))
        {
            auto it = ++tokens->begin();
            while (it != tokens->end())
            {
                Console::Printf("%s ", (const int8_t *)(*it));
                ++it;
            }
            Console::Printf("\n");
        }
        else if (!StringUtil::strcmp("ls", (const int8_t *)tokens->PeekFirst()))
        {
            if (DiskManager::GetDisk() != 0)
            {
                PartitionTable::ListFiles(DiskManager::GetDisk());
            }
            else
            {
                Console::Printf("No disk available\n");
            }
        }
        else if (!StringUtil::strcmp("cat", (const int8_t *)tokens->PeekFirst()))
        {
            auto it = ++tokens->begin();
            if (it != tokens->end())
            {
                if (DiskManager::GetDisk() != 0)
                {
                    auto contents = PartitionTable::ReadFileContents(DiskManager::GetDisk(), (const int8_t *)(*it));
                    if (contents != nullptr && contents[0] != '\0')
                    {
                        Console::Printf("%s\n", contents);
                    }
                    else
                    {
                        auto color = Console::SetColor(12);
                        Console::Printf("cat: %s: No such file\n", (const int8_t *)(*it));
                        Console::SetColor(color);
                    }
                }
                else
                {
                    Console::Printf("No disk available\n");
                }
            }
            else
            {
                Console::Printf("Usage: cat <filename>\n");
            }
        }
        else if (!StringUtil::strcmp("pwd", (const int8_t *)tokens->PeekFirst()))
        {
            Console::Printf("/\n");
        }
        else if (!StringUtil::strcmp("uptime", (const int8_t *)tokens->PeekFirst()))
        {
            uint32_t seconds = Console::ticks / 18; // ~18.2 ticks per second (PIT default)
            uint32_t minutes = seconds / 60;
            uint32_t hours = minutes / 60;
            Console::Printf("up %d:%02d:%02d\n", hours, minutes % 60, seconds % 60);
        }
        else if (!StringUtil::strcmp("reboot", (const int8_t *)tokens->PeekFirst()))
        {
            Console::Printf("Rebooting...\n");
            // Use keyboard controller to pulse CPU reset line
            Port8Bit kbdCmd(0x64);
            // Wait for keyboard controller to be ready
            uint8_t status;
            do {
                status = kbdCmd.Read();
            } while (status & 0x02);
            // Send reset command
            kbdCmd.Write(0xFE);
            // Halt if reset fails
            asm volatile("hlt");
        }
        else if (!StringUtil::strcmp("halt", (const int8_t *)tokens->PeekFirst()))
        {
            Console::Printf("System halted.\n");
            // QEMU ACPI shutdown
            Port16Bit acpiPort(0x604);
            acpiPort.Write(0x2000);
            // Fallback: halt CPU
            asm volatile("cli; hlt");
        }
        else if (!StringUtil::strcmp("nano", (const int8_t *)tokens->PeekFirst()))
        {
            auto it = ++tokens->begin();
            if (it != tokens->end())
            {
                int8_t *contents = nullptr;
                if (DiskManager::GetDisk() != 0)
                {
                    contents = PartitionTable::ReadFileContents(DiskManager::GetDisk(), (const int8_t *)(*it));
                }
                
                activeEditor = new Editor(keyboardDriver, this);
                activeEditor->Open((const int8_t *)(*it), contents);
                keyboardDriver->SetEventHandler(activeEditor);
            }
            else
            {
                // New file
                activeEditor = new Editor(keyboardDriver, this);
                activeEditor->Open("untitled", nullptr);
                keyboardDriver->SetEventHandler(activeEditor);
            }
        }
        else if (!StringUtil::strcmp("uname", (const int8_t *)tokens->PeekFirst()))
        {
            auto color = Console::SetColor(10);
            Console::Printf("MoOS kernel v0.1\n");
            Console::SetColor(color);
        }
        else if (!StringUtil::strcmp("lspci", (const int8_t *)tokens->PeekFirst()))
        {
            DriverManager driverManager;
            PeripheralComponentInterconnectController pciController;
            auto color = Console::SetColor(0xE);

            Console::Printf("Listing PCI device drivers ...\n\n");
            pciController.SelectDrivers(&driverManager, InterruptManager::ActivateInterruptManager);
            Console::SetColor(color);
        }
        else if (!StringUtil::strcmp("free", (const int8_t *)tokens->PeekFirst()))
        {
            auto color = Console::SetColor(6);
            auto it = ++tokens->begin();
            if (it != tokens->end() && !StringUtil::strcmp("-h", (const int8_t *)(*it)))
            {
                Console::Printf("%d KB remaining in heap\n", MemoryManager::activeMemoryManager->GetFree() / 1024);
            }
            else
            {
                Console::Printf("%d bytes remaining in heap\n", MemoryManager::activeMemoryManager->GetFree());
            }
            Console::SetColor(color);
        }
        else if (!StringUtil::strcmp("test", (const int8_t *)tokens->PeekFirst()))
        {
            auto color = Console::SetColor(6);
            auto it = ++tokens->begin();
            if (it != tokens->end())
            {
                OSTest::RunTest(StringUtil::atoi((const int8_t *)(*it)));
            }
            else
            {
                Console::Printf("Missing test number\n");
            }
            Console::SetColor(color);
        }
        else
        {
            if (tokens->PeekFirst() != 0)
            {
                Console::Printf("command not found: ");
                auto color = Console::SetColor(12);
                Console::Printf("'%s'\n", (const int8_t *)tokens->PeekFirst());
                Console::SetColor(color);
            }
        }

        auto copy = shared_ptr(copyBuffer());

        tokens->FreeList();
        
        // Only draw prompt if we're not in editor mode
        if (activeEditor == nullptr)
        {
            drawPrompt();
        }
        bufferCount = 0;
        cursorPos = 0;
    }
    break;

    default:
        auto c = keyToChar(key);
        if (c != '\0')
        {
            insertCharAtCursor(c);
            redrawInput();
        }
        break;
    }
}

void Terminal::OnKeyUp(Key key)
{
    switch (key)
    {
    case Key::RShift:
    case Key::LShift:
        isShiftDown = false;
        break;
    case Key::LControl:
    case Key::RControl:
        isCtrlDown = false;
        break;
    default:
        break;
    }
}

void Terminal::insertCharAtCursor(int8_t c)
{
    if (bufferCount >= 1999)
        return;
    
    // Shift characters right from cursor position
    for (int32_t i = bufferCount; i > cursorPos; i--)
        buffer[i] = buffer[i - 1];
    
    buffer[cursorPos] = c;
    bufferCount++;
    cursorPos++;
}

void Terminal::deleteCharAtCursor()
{
    if (cursorPos <= 0 || bufferCount <= 0)
        return;
    
    cursorPos--;
    // Shift characters left
    for (int32_t i = cursorPos; i < bufferCount - 1; i++)
        buffer[i] = buffer[i + 1];
    bufferCount--;
}

void Terminal::moveCursorLeft()
{
    if (cursorPos > 0)
        cursorPos--;
}

void Terminal::moveCursorRight()
{
    if (cursorPos < bufferCount)
        cursorPos++;
}

void Terminal::moveCursorToStart()
{
    cursorPos = 0;
}

void Terminal::moveCursorToEnd()
{
    cursorPos = bufferCount;
}

void Terminal::clearInput()
{
    // Move to prompt position and clear
    Console::SetCursor(promptX, promptY);
    for (int32_t i = 0; i < bufferCount + 1; i++)
        Console::PutChar(' ');
    Console::SetCursor(promptX, promptY);
    bufferCount = 0;
    cursorPos = 0;
}

void Terminal::redrawInput()
{
    // Move to prompt position
    Console::SetCursor(promptX, promptY);
    
    // Draw buffer content
    for (int32_t i = 0; i < bufferCount; i++)
        Console::PutChar(buffer[i]);
    
    // Clear any trailing characters (in case of deletion)
    Console::PutChar(' ');
    Console::PutChar(' ');
    
    // Position cursor at cursorPos
    Console::SetCursor(promptX + cursorPos, promptY);
}

void Terminal::Reset()
{
    drawPrompt();
}

void Terminal::drawPrompt()
{
    Console::SetColor(0x0A);  // Green
    Console::Print("> ");
    Console::SetColor(0x07);  // Back to gray
    promptY = Console::GetCursorY();
    promptX = Console::GetCursorX();
    cursorPos = 0;
    bufferCount = 0;
}

int8_t *Terminal::copyBuffer()
{
    size_t size = 0;
    while (buffer[size++] != '\0')
        ;

    auto bufferCopy = new int8_t[size];

    for (size_t i = 0; i < size; i++)
    {
        bufferCopy[i] = buffer[i];
    }

    return bufferCopy;
}

size_t Terminal::copyToBuffer(int8_t *src)
{
    size_t i = 0;

    while (src[i] != '\0')
    {
        buffer[i] = src[i];
        i++;
    }
    buffer[i] = '\0';

    return i;
}