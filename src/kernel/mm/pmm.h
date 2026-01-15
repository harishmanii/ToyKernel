#pragma once
#include <stdint.h>
#include <stddef.h>

struct memory_map_entry {
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi_attributes;
};

// Memory region types
#define MEMORY_USABLE 1
#define MEMORY_RESERVED 2
#define MEMORY_ACPI_RECLAIMABLE 3
#define MEMORY_ACPI_NVS 4
#define MEMORY_BAD 5


void init_memory(struct memory_map_entry* memory_map, uint32_t memory_map_count,char *__kernel__start,char *_kernel_end);

//helper functions
void setup_memory_bitmap(char *_kernel_end);
void reserve_memory_block(uint32_t base,uint32_t size);
void reserve_memory();
