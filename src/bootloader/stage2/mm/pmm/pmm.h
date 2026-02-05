#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../bootparams.h"
#include "../../stdio.h"
#include "../../string.h"


// Memory region types
#define MEMORY_USABLE 1
#define MEMORY_RESERVED 2
#define MEMORY_ACPI_RECLAIMABLE 3
#define MEMORY_ACPI_NVS 4
#define MEMORY_BAD 5
#define PER_BLOCK_SIZE 4096 
#define BYTE 8


#define INVALID_FRAME (-1)


void INIT_MEMORY(BootParams* bootParams);

//helper functions
void setup_memory_bitmap();
void reserve_memory_block(uint32_t base,uint32_t size);
void reserve_memory();
uintptr_t allocate_chunk();
uintptr_t allocate_page(size_t size);
