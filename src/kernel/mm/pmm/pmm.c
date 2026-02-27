#include "pmm.h"



static uint32_t TOTAL_MEMORY; // total memory in bytes
static uint32_t TOTAL_PAGES; // total no of 4kb pages
static uint32_t TOTAL_BLOCK; // is the number of uint32_t entries in your bitmap.
static uint32_t TOTAL_USED_BLOCKS = 0;


static uint32_t* BIT_MAP=0;  // Bit map start address
static uint32_t BITMAP_SIZEB; // total bytes needed to cover the entire memory address
static uint32_t BIT_MAP_END; // it represent the last pointer if the bitmap 

static MemoryInfo* MEMORY_MAP; // memory regions
static uintptr_t KERNEL_START_ADDR;
static uintptr_t KERNEL_END_ADDR;



extern char __kernel__start;
extern char __kernel__end;


void get_memory_info(MemoryInfo* memory)
{
     TOTAL_MEMORY = 0;
    for (int i = 0; i < memory->RegionCount; i++) {
        if (memory->Regions[i].Type == 1) {
            TOTAL_MEMORY += ((uint64_t)memory->Regions[i].Length_High << 32)
                            | memory->Regions[i].Length_Low;
        }
    }

    TOTAL_PAGES = TOTAL_MEMORY / PER_BLOCK_SIZE;
    BITMAP_SIZEB = (TOTAL_PAGES + 7) / BYTE;
    TOTAL_BLOCK = (TOTAL_PAGES + 31) / 32;
    MEMORY_MAP = memory;
}

void set_kernel_addr()
{
    KERNEL_START_ADDR = (uintptr_t)&__kernel__start;
    KERNEL_END_ADDR = (uintptr_t)&__kernel__end;
}

void set_bit(int bit)
{
    BIT_MAP[bit/32] |= (1 << (bit % 32));
}

void unset_bit(int bit)
{
    BIT_MAP[bit/32] &= ~(1 << (bit % 32));
}

int test_bit(uint32_t bit)
{
    return BIT_MAP[bit/32] & (1 << (bit % 32));
}

void setup_memory_bitmap()
{
    uint32_t bit_map_start  = ((uint32_t)KERNEL_END_ADDR + 0xFFF) & ~0xFFF; 
    BIT_MAP = (uint32_t*)bit_map_start;
    BIT_MAP_END = bit_map_start + BITMAP_SIZEB;
    memset((void*)bit_map_start, 0, BITMAP_SIZEB); // zeroing all the bit_map position for the initial time
}

int32_t get_free_memory_block()
{
    for(uint32_t i = 0;i<TOTAL_BLOCK;i++){
        if(BIT_MAP[i]!=0xffffffff){
            for(uint32_t j = 0 ;j<32;j++){
                uint32_t bit = i * 32 + j;    // exact bit
                if(!test_bit(bit)){
                    return bit;
                }
            }
        }
    }
    return INVALID_FRAME;
}

void reserve_memory_block(uintptr_t base,size_t size)
{
    uintptr_t start_page = base / PER_BLOCK_SIZE;
    size_t pages = (size + PER_BLOCK_SIZE - 1) / PER_BLOCK_SIZE;

      if(TOTAL_USED_BLOCKS >= TOTAL_BLOCK){
        return;
      }

    for (uint32_t i = 0; i < pages; i++) {
        // if (page >= MAX_PAGES)
        //     panic("reserve_memory_block: out of range");
        if (!test_bit(start_page + i)) {
            set_bit(start_page + i);
            TOTAL_USED_BLOCKS++;
        }
    }
}

void unreserve_memory_block(uint32_t base,uint32_t size)
{
    uint32_t start_page = base / PER_BLOCK_SIZE;
    uint32_t pages = (size + PER_BLOCK_SIZE - 1) / PER_BLOCK_SIZE;

    for (uint32_t i = 0; i < pages; i++) {
            unset_bit(start_page + i);
            TOTAL_USED_BLOCKS--;
    }
}

void reserve_memory()
{
    reserve_memory_block((uint32_t)0x0,(uint32_t)0x500);    // reserve lower memory region
    reserve_memory_block(0x7c00,512);  // reserve for bootloader
    reserve_memory_block(KERNEL_START_ADDR,(KERNEL_END_ADDR-KERNEL_START_ADDR));    // reserve for kernel section
    reserve_memory_block((uint32_t)BIT_MAP,BITMAP_SIZEB);   //reserve the memory BIT_MAP
}

uintptr_t allocate_chunk()
{
    //TODO: Need to track the no of blocks consumed, if no blocks less then 0 return -1 here
    // ASSUMES identity-mapped physical memory
    if(TOTAL_USED_BLOCKS >= TOTAL_BLOCK){
      return 0;
    }
    int32_t frame = get_free_memory_block();
    if (frame == INVALID_FRAME)
    return 0;
    set_bit(frame);
    uintptr_t addr = frame * PER_BLOCK_SIZE;
    TOTAL_USED_BLOCKS++;
    return addr;
}

uint32_t get_free_frame_by_size(size_t size)
{
  if(size==0){
    return INVALID_FRAME;
  }else if(TOTAL_USED_BLOCKS >= TOTAL_BLOCK){
    return INVALID_FRAME;
  }
  else if(size==1){
    return get_free_memory_block(); //it returns the frame no only
  }

  for(uint32_t i = 0 ; i < TOTAL_BLOCK ; i++){
    if(BIT_MAP[i]!=0xffffffff){
      for(uint32_t j = 0 ;j<32;j++){
        uint32_t bit = i * 32 + j;
        // need to work on this
        uint32_t free = 0;
        if(!test_bit(bit)){
          for(uint32_t count = 0;count<=size;count++){
            if(!test_bit(bit+count)){
              free++;
            }
            else{
              break;
            }
            if(free==size){
              return bit;
            } 
          }
        }
      }
    }
  }
  return INVALID_FRAME;
}


uintptr_t allocate_byte(size_t size)
{
  uint32_t frames = (size + PER_BLOCK_SIZE - 1) / PER_BLOCK_SIZE; // convert the size to exact no of fram so we can assign a byte
  uint32_t frame = get_free_frame_by_size(frames);
  
  if(frame == INVALID_FRAME)return 0;

  for(uint32_t i = 0; i < frames;i++){
    set_bit(frame+i);
    TOTAL_USED_BLOCKS++;
  }
  uintptr_t addr = frame*PER_BLOCK_SIZE;
  memset((void*)addr,0,size);
  return addr;
}

uintptr_t allocate_block(size_t size)
{
  uint32_t frame = get_free_frame_by_size(size);
  
  if(frame == INVALID_FRAME)return 0;

  for(uint32_t i = 0; i < size;i++){
    set_bit(frame+i);
    TOTAL_USED_BLOCKS++;
  }
  uintptr_t addr = frame*PER_BLOCK_SIZE;
  memset((void*)addr,0,size*PER_BLOCK_SIZE);
  return addr;
}

void deallocate_memory(uintptr_t base)
{
    uintptr_t frame = base / PER_BLOCK_SIZE;
    unset_bit(frame);
    TOTAL_USED_BLOCKS--;
}

int deallocate_memory_by_size(uint32_t base,uint32_t size)
{
    uint32_t pages = (size + PER_BLOCK_SIZE - 1) / PER_BLOCK_SIZE;
    uint32_t start = base / PER_BLOCK_SIZE;

    for (uint32_t i = 0; i < pages; i++) {
        unset_bit(start + i);
        TOTAL_USED_BLOCKS--;
    }
}


void INIT_MEMORY(BootParams* bootParams)
{
    set_kernel_addr();
    get_memory_info(&bootParams->Memory);
    setup_memory_bitmap();
    reserve_memory(); 

}


/*
TODO:
No free-count tracking
No double-free detection
No usable-region filtering - Right now you reserve manually. Acceptable for v0.
32-bit assumptions - You’re clearly in early x86 - fine.

Need to enchance all the functions because i created this for practing the physical memory management.
*/