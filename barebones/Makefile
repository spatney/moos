GCCPARAMS = -m32
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o kernel.o

%.o: %.cpp
	g++ $(GCCPARAMS) -c -o $@ $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

kernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)
	rm $(objects)

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
	rm -rf iso
	rm kernel.bin

run: kernel.iso
	(pkill VirtualBoxVM) || true
	VirtualBoxVM --startvm "Barebones" &

wsl-run: kernel.iso
	(powershell.exe "taskkill /IM "VirtualBoxVM.exe" /F") || true
	/mnt/c/Program\ Files/Oracle/VirtualBox/VirtualBoxVM.exe --startvm "Barebones" &

.phony: build-docker-image docker qemu qemu-run
build-docker-image:
	docker build --platform linux/x86-64 -t moos .
docker:
	docker run --rm -v "$$(pwd)":/moos --platform linux/x86-64 moos
qemu: 
	qemu-system-i386 -cdrom kernel.iso -vga virtio -m 1G
qemu-run: docker qemu

wsl-launch-qemu:
	(powershell.exe "taskkill /IM "qemu-system-i386.exe" /F") || true
	powershell.exe -File "c:\scripts\run2.ps1" &
wsl-run-qemu: kernel.iso wsl-launch-qemu