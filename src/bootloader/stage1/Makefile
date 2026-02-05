BUILD_DIR?=build/
TARGET_ASM?=nasm

.PHONY: all clean

all: stage1

stage1: $(BUILD_DIR)/bin/stage1.bin

$(BUILD_DIR)/bin/stage1.bin:
	$(ASM) boot.asm -f bin -o $@


clean:
	rm -f $(BUILD_DIR)/stage1.bin