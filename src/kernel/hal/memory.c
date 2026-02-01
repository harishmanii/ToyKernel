#include "memory.h"


void Initialize_memories(BootParams *bootParams) {
  // initialize physical memory region
  INIT_MEMORY(bootParams);
  // initialize virtual memory region
  // this function will give boolean result need to log those
  INITIALIZE_VMEMORY();
}