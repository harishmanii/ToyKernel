KERNEL_C=$(BUILD_DIR)/kernel/c
KERNEL_LIB=$(KERNEL_C)/lib
KERNEL_ASM=$(BUILD_DIR)/kernel/asm
KERNEL_MAIN_LINKER=linkers/linker.ld

KERNEL_ASM_ARCH=$(KERNEL_ASM)/arch
KERNEL_ASM_I686=$(KERNEL_ASM)/arch/i686


TARGET_ASM_FLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
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
	$(TARGET_LD) $(TARGET_LINKER_FLAGS) -Wl,-Map=$(DEBUGGING_BUILD_DIR)/kernel.map -o $@ $^ $(TARGET_LIBS)
	$(TARGET_OBJCOPY) -O binary $(DEBUGGING_BUILD_DIR)/kernel.elf $(BIN_BUILD_DIR)/kernel.bin

$(KERNEL_C)/%.o: %.c always
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(KERNEL_ASM)/%.o: %.asm always
	$(TARGET_ASM) $(TARGET_ASM_FLAGS) -o $@ $<

always:
	mkdir -p $(KERNEL_C)
	mkdir -p $(KERNEL_LIB)
	mkdir -p $(KERNEL_ASM)
	mkdir -p $(KERNEL_ASM_ARCH)
	mkdir -p $(KERNEL_ASM_I686)


clean:
	rm -f $(BIN_BUILD_DIR)/kernel.bin 