FROM ubuntu
RUN apt update
RUN apt install make g++ binutils libc6-dev-i386 grub-common xorriso grub-pc-bin zsh curl git -y
WORKDIR /moos
CMD ["make", "-j8", "kernel.iso"]