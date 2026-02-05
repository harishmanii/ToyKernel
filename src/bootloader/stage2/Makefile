

TARGET_ASM_FLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKER_FLAGS += -T linker.ld -nostdlib




SOURCES_C=$(shell find . -type f -name "*.c")
SOURCES_ASM=$(shell find . -type f -name "*.asm")
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/stage2/c/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/stage2/asm/%.o, $(SOURCES_ASM))


.PHONY: all stage2 clean always

all: stage2

stage2: $(BUILD_DIR)/bin/stage2.bin

$(BUILD_DIR)/debugging/stage2.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	@mkdir -p $(dir $@)
	$(TARGET_LD) $(TARGET_LINKER_FLAGS) \
	-Wl,-Map=$(BUILD_DIR)/debugging/stage2.map \
	-o $@ $^ $(TARGET_LIBS)

$(BUILD_DIR)/bin/stage2.bin: $(BUILD_DIR)/debugging/stage2.elf
	@mkdir -p $(dir $@)
	$(TARGET_OBJCOPY) -O binary $(BUILD_DIR)/debugging/stage2.elf $@

$(BUILD_DIR)/stage2/c/%.o: %.c 
	@mkdir -p $(dir $@)
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/stage2/asm/%.o: %.asm 
	@mkdir -p $(dir $@)
	$(TARGET_ASM) $(TARGET_ASM_FLAGS) -o $@ $<

clean:
	rm -f $(BUILD_DIR)/bin/stage2.bin