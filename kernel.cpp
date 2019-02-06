void printf(const char * msg) {
    unsigned short* VideoMemory = (unsigned short*)0xb8000;

    for(int i=0; msg[i] != '\0'; ++i) {
        VideoMemory[i] = (VideoMemory[i] & 0xFF00 | msg[i]);
    }
}

extern "C" void kernelMain(void * multiboot, unsigned int magic) {
    printf("Soon this will be an OS");

    while(1);
}