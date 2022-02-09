# MoOS

Attempting to implement a bare-metal OS, for the x86 processor architecture, from scratch. This project is just for fun, and so will not provide much (if any) value for folks looking for a more serious reference to OS design.

If you are looking for the most basic, hello world, type OS impelmentation then refer to the [barebones folder](barebones/).

[One time setup] `make build-docker-image` or `docker build --platform linux/x86-64 -t moos .` do this once to build a docker image. This image will be used to perform builds.

`make docker` or `docker run --rm -v "$(pwd)":/moos --platform linux/x86-64 moos` build a bootable iso with GRUB as the bootloader via docker. You can run the iso via a virtualization / emulation tool of your choice. Refer to the 'Makefile' for examples of virtual box and qemu.

## MoOS Terminal
__A basic shell with the ability to execute some commands.__

![MoOS Terminal Screenshot](https://envy.blob.core.windows.net/moos/moosterminal2.gif)

## MoOS in 'Graphics Mode'
__Just some window movement, doesn't offer any real functionality.__

![MoOS Terminal Screenshot](https://envy.blob.core.windows.net/moos/moosgfx2.gif)

## Useful commands
Some scripts / commands to help with the development workflow.

#### Creating a FAT32 volume with 'mtools' with a text file in it.

`dd if=/dev/zero of=disk.img bs=256m count=2` create a disk 256MB (min for FAT 32) in size

`mformat -F -i disk.img ::` format disk with FAT32

`mcopy -i disk.img example.txt ::` copy text file into image

`mdir -i disk.img ::` list files in root
