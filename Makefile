GCCPARAMS = -m32 -Iheaders -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fcheck-new -std=c++17
GCCPARAMSGFX = $(GCCPARAMS) -DGRAPHICS_MODE

ASPARAMS = --32
LDPARAMS = -melf_i386

default_objects = obj/loader.o \
		  obj/core/gdt.o \
		  obj/core/multitasking.o \
		  obj/core/memory.o \
		  obj/core/syscall.o \
		  obj/common/console.o \
		  obj/common/linkedlist.o \
		  obj/common/strings.o \
		  obj/hardware/port.o \
		  obj/hardware/interrupts_asm.o \
		  obj/hardware/interrupts.o \
		  obj/hardware/pci.o \
		  obj/drivers/driver.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
		  obj/drivers/vga.o \
		  obj/file_system/msdos_partition.o \
		  obj/file_system/fat.o \
		  obj/drivers/amd_am79c973.o \
		  obj/drivers/ata.o \
		  obj/gui/terminal.o \
		  obj/kernel.o

# gfx = 1
ifdef gfx
GCCFLAGS = $(GCCPARAMSGFX)
objects = $(default_objects) \
		  obj/gui/graphics.o \
		  obj/gui/widget.o \
		  obj/gui/window.o \
		  obj/gui/desktop.o
else
GCCFLAGS = $(GCCPARAMS)
objects = $(default_objects)
endif

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GCCFLAGS) -c -o $@ $<

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

.phony: clean wsl-launch
clean:
	rm -rf obj iso kernel.bin kernel.iso

run: kernel.iso
	(pkill VirtualBoxVM) || true
	VirtualBoxVM --startvm "MoOS" &

wsl-launch-qemu:
	(powershell.exe "taskkill /IM "qemu-system-i386.exe" /F") || true
	powershell.exe -File "c:\scripts\run.ps1" &
wsl-run-qemu: kernel.iso wsl-launch-qemu

wsl-launch:
	(powershell.exe "taskkill /IM "VirtualBoxVM.exe" /F") || true
	/mnt/c/Program\ Files/Oracle/VirtualBox/VirtualBoxVM.exe --startvm "MoOS" &
wsl-run: kernel.iso wsl-launch

.phony: build-docker-image docker qemu qemu-run qemu-disk-image

build-docker-image:
	docker build --platform linux/x86-64 -t moos .

docker:
	docker run --rm -v "$$(pwd)":/moos --platform linux/x86-64 moos

qemu: 
	qemu-system-i386 -boot d -cdrom kernel.iso -drive file=disk.img,format=raw -m 1G

qemu-none-graphics-mode: # use this if running within a container
	qemu-system-i386 -boot d -cdrom kernel.iso -drive file=disk.img,format=raw -m 1G -curses

qemu-run: qemu-disk-image docker qemu

qemu-disk-image:
	rm -f disk.img
	dd if=/dev/zero of=disk.img bs=25M count=2
	mformat -F -i disk.img ::
	mcopy -i disk.img file1.txt ::
	mcopy -i disk.img logo.txt ::
	mdir -i disk.img ::