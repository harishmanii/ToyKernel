#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../boot/bootparams.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "memory.h"


// Memory region types
#define MEMORY_USABLE 1
#define MEMORY_RESERVED 2
#define MEMORY_ACPI_RECLAIMABLE 3
#define MEMORY_ACPI_NVS 4
#define MEMORY_BAD 5

#define INVALID_FRAME (-1)


void INIT_MEMORY(BootParams* bootParams);

//helper functions
void setup_memory_bitmap();
void reserve_memory_block(uint32_t base,uint32_t size);
void reserve_memory();
