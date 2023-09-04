# the BUILD_DIR is passed down from top Makefile
BUILD_DIR?=build
ASM?=nasm

.PHONY: all stage2 clean

all: stage2

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin:
	$(ASM) main.asm -f bin -o $(BUILD_DIR)/stage2.bin

clean:
	rm -f $(BUILD_DIR)/stage2.bin