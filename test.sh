nasm boot.asm -f bin -o Binaries/boot.bin

nasm kernel_entry.asm -f elf32 -o Binaries/kernel_entry.o

gcc -m32 -ffreestanding -fno-pie -no-pie -c kernel.cpp -o Binaries/kernel.o

ld -m elf_i386 \
-Ttext 0x1000 \
-e _start \
-o Binaries/full_kernel.bin \
Binaries/kernel_entry.o \
Binaries/kernel.o \
--oformat binary

nasm zeroes.asm -f bin -o Binaries/zeroes.bin

cat "Binaries/boot.bin" "Binaries/full_kernel.bin" "Binaries/zeroes.bin"  > "Binaries/OS.bin"

qemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M
