all:
	nasm -f bin bootloader.asm -o bootloader.bin
	nasm -f elf32 kernel.asm -o kernel.o
	x86_64-elf-gcc -m32 -c ckernel.c -o ckernel.o
	x86_64-elf-ld -m elf_i386 -T kernel.ld kernel.o ckernel.o -o kernel.bin
	cat bootloader.bin kernel.bin > myOS.bin
	rm bootloader.bin ckernel.o kernel.o kernel.bin