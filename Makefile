GPPPARAMS = -m32
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o kernal.o

%.o: %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s 
	as $(ASPARAMS) -o $@ $<

mykernal.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernal.bin
	sudo cp $< /boot/mykernal.bin