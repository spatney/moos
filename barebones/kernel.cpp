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

    for (auto i = 0; message[i] != '\0'; ++i)
    {
        if (message[i] == ' ')
        {
            // switch color at wordbreak
            color = (color + 1) % 0x0F;
        }

        // first byte is color, second is the text
        videoMemory[i] = color << 8 | message[i];
    }
    while (1)
        ;
}