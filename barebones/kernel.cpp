extern "C" void kernel_main()
{
    const char *message = "Hello! this is the Barebones OS. Bye!";
    short *VideoMemory = (short *)0xb8000;

    for (int i = 0; message[i] != '\0'; ++i)
    {
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | message[i];
    }
    while (1)
        ;
}