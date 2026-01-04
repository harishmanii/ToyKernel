



IMAGE_DIR = $(BUILD_DIR)/image
export BUILD_DIR=build
include build_scripts/config.mk

.PHONY: all floppy_image kernel bootloader clean always tools_fat run debug

include build_scripts/toolchain.mk



all: floppy_image

#
# Floppy image
#
floppy_image: $(IMAGE_DIR)/main_floppy.img

$(IMAGE_DIR)/main_floppy.img: bootloader kernel
	echo "Building image..."
	dd if=/dev/zero of=$(IMAGE_DIR)/main_floppy.img bs=512 count=2880
	mformat -f 1440 -L 12 -v "NBOS" -i $(IMAGE_DIR)/main_floppy.img
	dd if=$(BUILD_DIR)/bin/stage1.bin of=$(IMAGE_DIR)/main_floppy.img conv=notrunc
	mcopy -i $(IMAGE_DIR)/main_floppy.img -mv $(BUILD_DIR)/bin/stage2.bin "::stage2.bin"
	mcopy -i $(IMAGE_DIR)/main_floppy.img -mv $(BUILD_DIR)/bin/kernel.bin "::kernel.bin"

#
# Bootloader
#
bootloader: stage1 stage2

stage1: $(BUILD_DIR)/bin/stage1.bin

$(BUILD_DIR)/bin/stage1.bin: always
	$(MAKE) -C src/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: $(BUILD_DIR)/bin/stage2.bin

$(BUILD_DIR)/bin/stage2.bin: always
	$(MAKE) -C src/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#
kernel: $(BUILD_DIR)/bin/kernel.bin

$(BUILD_DIR)/bin/kernel.bin: always
	$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Always
#
always:
	mkdir -p $(IMAGE_DIR)
	mkdir -p $(BUILD_DIR)/bin
	mkdir -p $(BUILD_DIR)/debugging

#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)/*

debug: all
	qemu-system-i386 -fda $(IMAGE_DIR)/main_floppy.img -S -s

debugb: clean all
	bochs -f debugging/bochs_config -dbg

gdbk:
	gdb -x debugging/kernel

gdb2:
	gdb -x debugging/stage2

run: clean all
	qemu-system-i386 -fda $(IMAGE_DIR)/main_floppy.img