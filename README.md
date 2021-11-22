# MoOS

Nothing special, just me trying to figure out how things work to better understand OS design. 

[One time setup] ```make build-docker-image``` or ```docker build --platform linux/x86-64 -t moos .``` do this once to build a docker image. This image will be used to perform builds.

```make docker``` or ```docker run --rm -v "$(pwd)":/moos --platform linux/x86-64 moos``` build a bootable iso with GRUB as the bootloader via docker. You can run the iso via a virtualization / emulation tool of your choice. Refer to the 'Makefile' for examples of virtual box and qemu.

## MoOS Terminal
![MoOS Terminal Screenshot](https://envy.blob.core.windows.net/moos/moos.png)

## MoOS is 'Graphics Mode'
#### Just some window movement, doesn't offer any real functionality

![MoOS Terminal Screenshot](https://envy.blob.core.windows.net/moos/moosgfx2.gif)
