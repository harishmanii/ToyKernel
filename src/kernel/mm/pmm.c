#include "pmm.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include <stdbool.h>
#include "memory.h"


static uint32_t TOTAL_MEMORY; // total memory in bytes
static uint32_t TOTAL_PAGES; // total no of 4kb pages
static uint32_t TOTAL_BLOCK; // is the number of uint32_t entries in your bitmap.

static uint32_t* BIT_MAP=0;  // Bit map start address
static uint32_t BITMAP_SIZEB; // total bytes needed to cover the entire memory address
static uint32_t BIT_MAP_END; // it represent the last pointer if the bitmap 

static struct memory_map_entry* MEMORY_MAP; // memory regions
static uint32_t KERNEL_START_ADDR;
static uint32_t KERNEL_END_ADDR;


void get_memory_info(struct memory_map_entry* memory_map, uint32_t memory_map_count)
{
    for (int i = 0; i < memory_map_count; i++) {
        if (memory_map[i].type == 1) {
            TOTAL_MEMORY += ((uint64_t)memory_map[i].length_high << ARCH)
                            | memory_map[i].length_low;
        }
    }

    TOTAL_PAGES = TOTAL_MEMORY / PER_BLOCK_SIZE;
    BITMAP_SIZEB = (TOTAL_PAGES + 7) / BYTE;
    TOTAL_BLOCK = (TOTAL_PAGES + 31) / ARCH;
    MEMORY_MAP = memory_map;
}

void set_kernel_addr(char *__kernel__start,char *__kernel__end)
{
    KERNEL_START_ADDR = (uint32_t) __kernel__start;
    KERNEL_END_ADDR = (uint32_t) __kernel__end;
}

void set_bit(int bit)
{
    BIT_MAP[bit/ARCH] |= (1 << (bit % ARCH));
}

void unset_bit(int bit)
{
    BIT_MAP[bit/ARCH] &= ~(1 << (bit % ARCH));
}

int test_bit(int bit)
{
    return (BIT_MAP[bit/ARCH] & (1 << (bit % ARCH)))==false?false:true;
}

void setup_memory_bitmap(char *_kernel_end)
{
   
    uint32_t bit_map_start  = ((uint32_t)_kernel_end + 0xFFF) & ~0xFFF; 
    BIT_MAP = (uint32_t*)bit_map_start;
    BIT_MAP_END = bit_map_start + BITMAP_SIZEB;

    memset((void*)bit_map_start, 0, BITMAP_SIZEB); // zeroing all the bit_map position for the initial time
}

int get_free_memory_block()
{
    for(uint32_t i = 0;i<TOTAL_BLOCK;i++){
        if(BIT_MAP[i]!=0xffffffff){
            for(uint32_t j = 0 ;j<ARCH;j++){
                uint32_t bit = i * ARCH + j;
                if(!test_bit(bit)){
                    return bit;
                }
            }
        }
    }
    return -1;
}

void reserve_memory_block(uint32_t base,uint32_t size)
{
    uint32_t start_page = base / PER_BLOCK_SIZE;
    uint32_t pages = (size + PER_BLOCK_SIZE - 1) / PER_BLOCK_SIZE;

    for (uint32_t i = 0; i < pages; i++) {
        if (!test_bit(start_page + i)) {
            set_bit(start_page + i);
        }
    }
}

void unreserve_memory_block(uint32_t base,uint32_t size)
{
    uint32_t start_page = base / PER_BLOCK_SIZE;
    uint32_t pages = (size + PER_BLOCK_SIZE - 1) / PER_BLOCK_SIZE;

    for (uint32_t i = 0; i < pages; i++) {
            unset_bit(start_page + i);
    }
}

void reserve_memory()
{
    reserve_memory_block((uint32_t)0x0,(uint32_t)0x500);    // reserve lower memory region
    reserve_memory_block(0x7c00,512);  // reserve for bootloader
    // 0x00500–0x07BFF: Free real-mode area
    reserve_memory_block(KERNEL_START_ADDR,(KERNEL_END_ADDR-KERNEL_START_ADDR));    // reserve for kernel section
    reserve_memory_block((uint32_t)BIT_MAP,BITMAP_SIZEB);   //reserve the memory BIT_MAP
}

void* allocate_memory()
{
    //TODO: Need to track the no of blocks consumed, if no blocks less then 0 return -1 here
    int frame = get_free_memory_block();
    if(frame==-1)return 0;
    set_bit(frame);

    uint32_t addr = frame * PER_BLOCK_SIZE;
    memset((void*)addr, 'A', PER_BLOCK_SIZE); 
    return (void*) addr;
}

int deallocate_memory(uint32_t base,uint32_t size)
{
    for(uint32_t i = 0; i < size; i++) {
        int frame = (base / PER_BLOCK_SIZE) + i;
        unset_bit(frame);
    }

}


void init_memory(struct memory_map_entry* memory_map, uint32_t memory_map_count,char *__kernel__start,char *_kernel_end)
{
    set_kernel_addr(__kernel__start,_kernel_end);
    get_memory_info(memory_map,memory_map_count);
    setup_memory_bitmap(_kernel_end);
    reserve_memory();
}


//TODO: Need to track the no of blocks consumed.
//TODO: Need a check for reserved region so no program can accidently change unset the memory.
//TODO: Need to enchance all the functions because i created this for practing the physical memory management.