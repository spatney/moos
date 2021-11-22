# MoOS

Attempting to implement a bare-metal OS, for the x86 processor architecture, from scratch. This project is just for fun, and so will not provide much (if any) value for folks looking for a more serious reference to OS design.

[One time setup] ```make build-docker-image``` or ```docker build --platform linux/x86-64 -t moos .``` do this once to build a docker image. This image will be used to perform builds.

```make docker``` or ```docker run --rm -v "$(pwd)":/moos --platform linux/x86-64 moos``` build a bootable iso with GRUB as the bootloader via docker. You can run the iso via a virtualization / emulation tool of your choice. Refer to the 'Makefile' for examples of virtual box and qemu.

## MoOS Terminal
#### A basic shell with the ability to execute some commands.
![MoOS Terminal Screenshot](https://envy.blob.core.windows.net/moos/moosterminal2.gif)

## MoOS in 'Graphics Mode'
#### Just some window movement, doesn't offer any real functionality.

![MoOS Terminal Screenshot](https://envy.blob.core.windows.net/moos/moosgfx2.gif)
