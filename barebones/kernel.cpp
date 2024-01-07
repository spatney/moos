// stuff written here gets written to console
short *videoMemory = ((short *)0xb8000);

void clearScreen()
{
    // clear the screen (80 columns, 25 rows represented consecutively)
    for (auto i = 0; i < (80 * 25); i++)
    {
        videoMemory[i] = 0 | ' ';
    }
}

extern "C" void kernel_main()
{
    // remove the text the bootloader prints to the screen.
    clearScreen();

    // we are going to try and print this to the screen
    auto message = "Hello! this is the Barebones OS. Bye!";
    auto color = 0x08;
    auto skip_lines = 80 * 2;

    for (auto lines = 0; lines < 10; ++lines)
    {
        for (auto i = 0; message[i] != '\0'; ++i)
        {
            if (message[i] == ' ')
            {
                color = (color + 1) % 0x0f;
                if (color == 14)
                    color++;
            }
            // first byte is the character, second byte is the color
            videoMemory[i + skip_lines] = color << 8 | message[i];
        }
        skip_lines += 80;
    }
    while (1)
        ;
}