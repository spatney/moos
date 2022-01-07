void clearScreen() {
    for (int i = 0; i < 80; i++)
    {
        for (int j = 0; j < 25; j++)
        {
            ((short *)0xb8000)[i + j*80] = 0 | ' ';
        }
    }
}

extern "C" void kernel_main()
{
    clearScreen();
    const char *message = "Hello! this is the Barebones OS. Bye!";

    for (int i = 0; message[i] != '\0'; ++i)
    {
        // 2bytes per char, one for color, one for text
        short color = 0x0A00;
        short colorAlt = 0x0C00;
        ((short *)0xb8000)[i] = (i % 2 ? color : colorAlt) | message[i];
    }
    while (1)
        ;
}