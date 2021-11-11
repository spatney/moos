extern "C" void kernel_main()
{
    const char *message = "Hello! this is the Barebones OS. Bye!";

    for (int i = 0; message[i] != '\0'; ++i)
    {
        // 2bytes per char, one for color, one for text
        short color = 0xF300;
        short colorAlt = 0xFF00;
        ((short *)0xb8000)[i] = (((short *)0xb8000)[i] & (i % 2 ? color : colorAlt)) | message[i];
    }
    while (1)
        ;
}