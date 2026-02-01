



IMAGE_DIR = $(BUILD_DIR)/image
export BUILD_DIR=build
export BIN_BUILD_DIR=$(BUILD_DIR)/bin
export DEBUGGING_BUILD_DIR=$(BUILD_DIR)/debugging

include build_scripts/config.mk

.PHONY: all floppy_image kernel bootloader clean always tools_fat run debug

include build_scripts/toolchain.mk



all: floppy_image

#
# Floppy image
#
floppy_image: $(IMAGE_DIR)/main_floppy.img

$(IMAGE_DIR)/main_floppy.img: bootloader kernel
	@mkdir -p $(dir $@)
	echo "Building image..."
	dd if=/dev/zero of=$(IMAGE_DIR)/main_floppy.img bs=512 count=2880
	mformat -f 1440 -L 12 -v "NBOS" -i $(IMAGE_DIR)/main_floppy.img
	dd if=$(BIN_BUILD_DIR)/stage1.bin of=$(IMAGE_DIR)/main_floppy.img conv=notrunc
	mcopy -i $(IMAGE_DIR)/main_floppy.img -mv $(BIN_BUILD_DIR)/stage2.bin "::stage2.bin"
	mcopy -i $(IMAGE_DIR)/main_floppy.img -mv $(BIN_BUILD_DIR)/kernel.bin "::kernel.bin"

#
# Bootloader
#
bootloader: stage1 stage2

stage1: $(BIN_BUILD_DIR)/stage1.bin

$(BIN_BUILD_DIR)/stage1.bin:
	@mkdir -p $(dir $@)
	$(MAKE) -C src/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: $(BIN_BUILD_DIR)/stage2.bin

$(BIN_BUILD_DIR)/stage2.bin: always
	$(MAKE) -C src/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#
kernel: $(BIN_BUILD_DIR)/kernel.bin

$(BIN_BUILD_DIR)/kernel.bin:
	$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Always
#

#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)/*

debug: clean all
	qemu-system-i386 -fda $(IMAGE_DIR)/main_floppy.img -S -s

debugb: clean all
	bochs -f debugging/bochs_config -dbg

gdbk:
	gdb -x debugging/kernel

gdb2:
	gdb -x debugging/stage2

build: clean all 

run: clean all
	qemu-system-i386 -fda $(IMAGE_DIR)/main_floppy.img