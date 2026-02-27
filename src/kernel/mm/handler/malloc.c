/*
malloc.c
Ref : Queso Fuego YT
This allocator uses single linked list dsa to allocate memory. Initilally in the kernel startup we will setup the virtual address starting point and size of heap as single linked list.
then if a user program needs memory it simply calls the malloc to get the memory from the linkedlist as we initialized in the bootup.
*/
#define ALIGN4(x) (((x) + 3) & ~3)

#include "malloc.h"

malloc_block_t *malloc_list_head = 0;    // Start of linked list
// this will changed on second stage
uint32_t malloc_virt_address     = 0;
uint32_t malloc_phys_address     = 0;
uint32_t total_malloc_pages      = 0;


// It will called from malloc function when there is no heap memory 
void malloc_init(const uint32_t bytes)
{
    total_malloc_pages = (bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    if (bytes % PAGE_SIZE > 0) total_malloc_pages++;    // Partial page

    // TODO: Change to allocate individual pages in case available physical memory is not contiguous
    malloc_phys_address = (uint32_t)allocate_block(total_malloc_pages);
    malloc_list_head    = (malloc_block_t *)malloc_virt_address;

    // Map in pages
    for (uint32_t i = 0, virt = malloc_virt_address; i < total_malloc_pages; i++, virt += PAGE_SIZE) {
        map_page((void *)(malloc_phys_address + i*PAGE_SIZE), (void *)virt);
        pt_entry *page = get_page(virt);
        SET_ATTRIBUTE(page, PTE_READ_WRITE); // Read/write access for malloc-ed pages
    }

    if (malloc_list_head) {
        // here we minus the header size as well because in each allocation we gave the header info aswell so if didn't minus it will cause the corrption
        malloc_list_head->size = (total_malloc_pages * PAGE_SIZE) - sizeof(malloc_block_t);
        malloc_list_head->free = true;
        malloc_list_head->next = 0;
    }
}



// It will be called if we want new node to be instered , here we simply gave ths size only
void malloc_split(malloc_block_t *node, uint32_t size)
{
    // size = ALIGN4(size);
    malloc_block_t *new_node = (malloc_block_t *)((void *)node + size + sizeof(malloc_block_t));

    new_node->size = node->size - size - sizeof(malloc_block_t);
    new_node->free = true;
    new_node->next = node->next;

    node->size = size;
    node->free = false;
    node->next = new_node;
}

void heap_dump(void)
{
    printf("heap dump: head=%p total_pages=%u virt_base=0x%x\n",
           malloc_list_head, total_malloc_pages, malloc_virt_address);
    malloc_block_t *it = malloc_list_head;
    uint32_t HEADER_SIZE =  sizeof(malloc_block_t);
    while (it) {
        printf("block %p: size=%u free=%d next=%p payload=%p end=%p\n",
               it, it->size, it->free, it->next,
               (uint8_t*)it + HEADER_SIZE,
               (uint8_t*)it + HEADER_SIZE + it->size);
        it = it->next;
    }
}


// Find & allocate next block of memory
void *malloc_next_block(const uint32_t size)
{
    malloc_block_t *cur = 0, *prev = 0; 
    // Nothing to malloc
    if (size == 0) return 0;

    // If no bytes in list to malloc, init it first
    if (!malloc_list_head->size) malloc_init(size);

    // Find first available block in list
    cur = malloc_list_head;
    while (((cur->size < size) || !cur->free) && cur->next) {
        prev = cur;
        cur = cur->next;
    }
    if (size == cur->size) {
        // Node size is what we are looking for
        cur->free = false;
    } else if (cur->size > size + sizeof(malloc_block_t)) {
        // Node is larger than what we need, split it into 2 nodes,
        //   will return current node for requested size
        malloc_split(cur, size);
    } else {
        // Node is too small, allocate more pages to current node, and split
        //   to return a node of requested size
        uint8_t num_pages = 1;
        while (cur->size + num_pages*PAGE_SIZE < size + sizeof(malloc_block_t))
            num_pages++;

        // Allocate & map in new pages starting at next virtual address page boundary (next 0x1000/4KB)
        uint32_t virt = malloc_virt_address + total_malloc_pages*PAGE_SIZE;

        for (uint8_t i = 0; i < num_pages; i++) {
            pt_entry page = 0;
            uint32_t *temp = allocate_page(&page);

            map_page((void *)temp, (void *)virt);
            SET_ATTRIBUTE(&page, PTE_READ_WRITE);
            virt += PAGE_SIZE;
            cur->size += PAGE_SIZE;
            total_malloc_pages++;
        }
        malloc_split(cur, size);
    }       
    // Return node pointing to requested bytes, casted to void * to remove pointer
    //   arithmetic
    return (void *)cur + sizeof(malloc_block_t);
}


// Merge consecutive free list nodes to prevent (some) memory fragmentation
void merge_free_blocks(void)
{
    malloc_block_t *cur = malloc_list_head;

    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += (cur->next->size) + sizeof(malloc_block_t);
            cur->next = cur->next->next;
        }
        cur = cur->next;
    }
}

void malloc_free(void *ptr)
{
    for (malloc_block_t *cur = malloc_list_head; cur->next; cur = cur->next) 
        if ((void *)cur + sizeof(malloc_block_t) == ptr) {
            cur->free = true;
            merge_free_blocks();
            break;
        }
}