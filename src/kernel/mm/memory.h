#define KERNEL_ADDRESS    0x100000     // kernel physical load address (1 MB)
#define KERNEL_VIRT_BASE  0xC0000000   // kernel higher-half virtual base

/*
 * Convert a kernel higher-half virtual address to its physical address.
 * The linker maps phys 0x100000 → virt 0xC0000000, so:
 *   phys = virt - KERNEL_VIRT_BASE + KERNEL_ADDRESS
 */
#define VIRT_TO_PHYS(v)   ((uint32_t)(v) - KERNEL_VIRT_BASE + KERNEL_ADDRESS)