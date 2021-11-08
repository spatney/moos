extern "C" void kernel_main()
{
    const char *message = "Hello! this is the Barebones OS. Bye!";

    for (int i = 0; message[i] != '\0'; ++i)
    {
        ((short *)0xb8000)[i] = (((short *)0xb8000)[i] & 0xFF00) | message[i];
    }
    while (1)
        ;
}