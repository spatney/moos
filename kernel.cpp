int cursor=0;

void printf(const char * msg) {
    unsigned short* VideoMemory = (unsigned short*)0xb8000;

    for(int i=0; msg[i] != '\0'; ++i) {
        VideoMemory[cursor] = (VideoMemory[cursor] & 0xFF00 | msg[i]);
        cursor++;
    }
}
char* itoa(int val, int base){

    static char buf[32] = {0};

    int i = 30;

    buf[i--] = '\0';

    for(; val && i ; --i, val /= base)

        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i+1];

}

extern "C" void kernelMain(void * multiboot, unsigned int magic) {
    printf("Soon this will be an OS: ");
    printf(itoa(magic, 16));

    while(1);
}