# Userland build – produces an ELF32 executable that the bootloader copies
# onto the FAT floppy image as "userland.elf" (8.3 name – FAT12 limit).
# The kernel's elf_load() parser reads ELF program headers at runtime.
#
# Inherited from root Makefile via export:
#   BUILD_DIR, TARGET_CC
#
# Source layout:
#   programs/user_land/user_land.c  – main event loop (ring 3, stays in userland)
#   lib/syscall.c                   – syscall wrappers (fire int $0x80 when needed)

USERLAND_OUT    = $(BUILD_DIR)/userland
USERLAND_CFLAGS = -std=c99 -ffreestanding -nostdlib -mgeneral-regs-only -g

USER_LAND_SRCS = $(shell find . -name "*.c" | sed 's|^\./||')
USER_LAND_ELF  = $(USERLAND_OUT)/userland.elf

.PHONY: all clean

all: $(USER_LAND_ELF)

# Compile + link both sources directly to ELF32 using the userland linker script.
# The kernel reads this ELF at runtime via elf_load() – no objcopy wrapping.
$(USER_LAND_ELF): $(USER_LAND_SRCS) linker/linker.ld | $(USERLAND_OUT)
	$(TARGET_CC) $(USERLAND_CFLAGS) -Ilib -T linker/linker.ld -o $@ $(USER_LAND_SRCS) -lgcc

$(USERLAND_OUT):
	@mkdir -p $@

clean:
	rm -f $(USER_LAND_ELF)
