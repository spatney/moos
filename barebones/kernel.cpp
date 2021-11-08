extern "C" void kernel_main()
{
    const char *message = "Hello! this is the Barebones OS. Bye!";
    short *VideoMemory = (short *)0xb8000;

    int x = 0;
    for (int i = 0; message[i] != '\0'; ++i)
    {
        VideoMemory[x] = (VideoMemory[x] & 0xFF00) | message[i];
        x++;
    }
    while (1)
        ;
}