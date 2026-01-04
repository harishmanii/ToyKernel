

TARGET_ASM_FLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKER_FLAGS += -T linker.ld -nostdlib



SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/stage2/c/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/stage2/asm/%.o, $(SOURCES_ASM))

.PHONY: all stage2 clean always

all: stage2

stage2: $(BUILD_DIR)/bin/stage2.bin

$(BUILD_DIR)/debugging/stage2.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	$(TARGET_LD) $(TARGET_LINKER_FLAGS) \
	-Wl,-Map=$(BUILD_DIR)/debugging/stage2.map \
	-o $@ $^ $(TARGET_LIBS)

$(BUILD_DIR)/bin/stage2.bin: $(BUILD_DIR)/debugging/stage2.elf
	$(TARGET_OBJCOPY) -O binary $(BUILD_DIR)/debugging/stage2.elf $@

$(BUILD_DIR)/stage2/c/%.o: %.c always
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/stage2/asm/%.o: %.asm always
	$(TARGET_ASM) $(TARGET_ASM_FLAGS) -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/stage2/c
	mkdir -p $(BUILD_DIR)/stage2/asm
	mkdir -p $(BUILD_DIR)/bin
	mkdir -p $(BUILD_DIR)/debugging

clean:
	rm -f $(BUILD_DIR)/bin/stage2.bin