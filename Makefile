# =============================================================================
# Variables

# Build tools
NASM = nasm
CC = gcc
FLAGS = -std=c99 -m32 -O2 -ffreestanding -no-pie -fno-pie -mno-sse -fno-stack-protector

BIN_FLAG = -f bin
ELF_FLAG = -felf
# =============================================================================
# Tasks

all: clean build test

build: c_compilation asm_compilation linking reduce_elf os.img

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

test: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda os.img -monitor stdio -device VGA

debug: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda os.img -monitor stdio -device VGA -s -S &
	gdb

.PHONY: all build clean test debug

c_compilation: src/kernel.c
	$(CC) $(FLAGS) -c src/kernel.c -o .tmp/kernel.o

asm_compilation: src/boot.asm
	$(NASM) $(ELF_FLAG) -DKERNEL_SIZE=8192 src/boot.asm -o .tmp/boot.o

linking: .tmp/boot.o .tmp/kernel.o
	ld -m elf_i386 .tmp/boot.o .tmp/kernel.o -T ./linker/link.ld -o .tmp/os.elf

reduce_elf: .tmp/os.elf
	objcopy -I elf32-i386 -O binary .tmp/os.elf .tmp/os.bin

os.img: .tmp/os.bin
	dd if=/dev/zero of=os.img bs=1024 count=1440
	dd if=.tmp/os.bin of=os.img conv=notrunc