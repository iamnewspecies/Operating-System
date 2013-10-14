#!/bin/sh
#http://wiki.osdev.org/Detecting_Memory_%28x86%29#Memory_Map_Via_GRUB
#http://irc.lc/freenode/osdev/newspecies@@@
export PREFIX="$HOME/cross"
export TARGET=i586-elf
export PATH="$PREFIX/bin:$PATH" 
i586-elf-as boot.s -o boot.o
i586-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i586-elf-gcc -T linker.ld -o RangaoS.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
mkdir -p isodir
mkdir -p isodir/boot
cp RangaoS.bin isodir/boot/RangaoS.bin
mkdir -p isodir/boot/grub
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o RangaoS.iso isodir