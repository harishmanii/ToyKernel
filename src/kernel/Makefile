

TARGET_ASM_FLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKER_FLAGS += -T linker.ld -nostdlib



SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/kernel/c/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/kernel/asm/%.o, $(SOURCES_ASM))

.PHONY: all kernel clean always

all: kernel

kernel: $(BUILD_DIR)/debugging/kernel.elf

$(BUILD_DIR)/debugging/kernel.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	$(TARGET_LD) $(TARGET_LINKER_FLAGS) -Wl,-Map=$(BUILD_DIR)/debugging/kernel.map -o $@ $^ $(TARGET_LIBS)
	$(TARGET_OBJCOPY) -O binary $(BUILD_DIR)/debugging/kernel.elf $(BUILD_DIR)/bin/kernel.bin

$(BUILD_DIR)/kernel/c/%.o: %.c always
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/kernel/asm/%.o: %.asm always
	$(TARGET_ASM) $(TARGET_ASM_FLAGS) -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/kernel/c
	mkdir -p $(BUILD_DIR)/kernel/asm

clean:
	rm -f $(BUILD_DIR)/bin/kernel.bin 