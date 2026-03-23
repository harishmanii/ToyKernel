#include "memory.h"



void Initialize_memories(BootParams *bootParams) {
  // initialize physical memory region
  INIT_MEMORY(bootParams);
  // initialize virtual memory region
  // this function will give boolean result need to log those
  INITIALIZE_VMEMORY();
  // malloc_virt_address is initialised to KERNEL_HEAP_START in malloc.c
  // malloc_phys_address = 0;
  // total_malloc_pages  = KERNEL_INITIAL_SIZE;

  // malloc_init();
}