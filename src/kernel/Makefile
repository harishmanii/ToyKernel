KERNEL_C=$(BUILD_DIR)/kernel/c
KERNEL_ASM=$(BUILD_DIR)/kernel/asm
KERNEL_MAIN_LINKER=linkers/kernel.ld

TARGET_ASM_FLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib -mgeneral-regs-only
TARGET_LIBS += -lgcc
TARGET_LINKER_FLAGS += -T $(KERNEL_MAIN_LINKER) -nostdlib


SOURCES_C=$(shell find . -type f -name "*.c")
SOURCES_ASM=$(shell find . -type f -name "*.asm")
OBJECTS_C=$(patsubst %.c, $(KERNEL_C)/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(KERNEL_ASM)/%.o, $(SOURCES_ASM))

.PHONY: all kernel clean always

all: kernel

kernel: $(BUILD_DIR)/debugging/kernel.elf

$(BUILD_DIR)/debugging/kernel.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	@mkdir -p $(dir $@)
	$(TARGET_LD) $(TARGET_LINKER_FLAGS) -Wl,-Map=$(DEBUGGING_BUILD_DIR)/kernel.map -o $@ $^ $(TARGET_LIBS)
	$(TARGET_OBJCOPY) -O binary $(DEBUGGING_BUILD_DIR)/kernel.elf $(BIN_BUILD_DIR)/kernel.bin

$(KERNEL_C)/%.o: %.c
	@mkdir -p $(dir $@)
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(KERNEL_ASM)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(TARGET_ASM) $(TARGET_ASM_FLAGS) -o $@ $<


clean:
	rm -f $(BIN_BUILD_DIR)/kernel.bin 