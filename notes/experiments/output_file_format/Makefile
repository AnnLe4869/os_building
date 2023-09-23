ASM = nasm
ASM_FLAGS = -f elf64
GCC = gcc
GCC_FLAGS = -std=c99 -g -ffreestanding -nostdlib -mno-red-zone 
GCC_FLAGS_NON = -std=c99 -g -ffreestanding

LD = gcc
LD_FLAGS = -nostdlib
LD_FLAGS_NON =
TARGET_LIBS += -lgcc

PHONY: all linker

all: linker

main.o: main.c
	$(GCC) -c $(GCC_FLAGS) -o main.o main.c 

x86.o: x86.asm
	$(ASM) $(ASM_FLAGS) -o x86.o $<

linker: main.o x86.o 
	$(LD) $(LD_FLAGS) -Wl,-Map=mapper.map -o final $^ $(TARGET_LIBS)