#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../pmm/pmm.h"
#include "../vmm/vmm.h"
#include "../../include/stdio.h"


//variable declration
#define PAGE_SIZE          4096
#define KERNEL_HEAP_START  0xD1000000
#define KERNEL_HEAP_MAX    0xD2000000
// Singly linked list nodes for blocks of memory
typedef struct malloc_block {
    uint32_t size;  // Size of this memory block in bytes
    bool free;      // Is this block of memory free?
    struct malloc_block *next;  // Next block of memory
} malloc_block_t;

extern malloc_block_t *malloc_list_head;    // Start of linked list
extern uint32_t malloc_virt_address;
extern uint32_t malloc_phys_address;
extern uint32_t total_malloc_pages;

void malloc_init(const uint32_t bytes);
void malloc_split(malloc_block_t *node, uint32_t size);
void *malloc_next_block(const uint32_t size);
void merge_free_blocks(void);
void malloc_free(void *ptr);

