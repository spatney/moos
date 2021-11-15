GCCPARAMS = -m32 -Iheaders -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fcheck-new -std=c++17
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		  obj/core/gdt.o \
		  obj/core/multitasking.o \
		  obj/core/memory.o \
		  obj/common/console.o \
		  obj/common/linkedlist.o \
		  obj/hardware/port.o \
		  obj/hardware/interruptstubs.o \
		  obj/hardware/interrupts.o \
		  obj/hardware/pci.o \
		  obj/drivers/driver.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
		  obj/drivers/vga.o \
		  obj/gui/terminal.o \
		  obj/gui/graphics.o \
		  obj/gui/widget.o \
		  obj/gui/window.o \
		  obj/gui/desktop.o \
		  obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

kernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)
	rm $(objects)

install: kernel.bin
	sudo cp $< /boot/kernel.bin

kernel.iso: kernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "MoOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf obj
	rm -rf iso
	rm kernel.bin

.phony: clean
clean:
	rm -rf obj iso kernel.bin kernel.iso

run: kernel.iso
	(pkill VirtualBoxVM) || true
	VirtualBoxVM --startvm "MoOS" &

wsl-run: kernel.iso
	(powershell.exe "taskkill /IM "VirtualBoxVM.exe" /F") || true
	/mnt/c/Program\ Files/Oracle/VirtualBox/VirtualBoxVM.exe --startvm "MoOS" &

.phony: build-docker-image docker qemu qemu-run
build-docker-image:
	docker build --platform linux/x86-64 -t moos .
docker:
	docker run --rm -v "$$(pwd)":/moos --platform linux/x86-64 moos
qemu: 
	qemu-system-i386 -cdrom kernel.iso -vga virtio -m 1G
qemu-run: docker qemu