#pragma once
#include "../boot/bootparams.h"
#include "../mm/pmm/pmm.h"
#include "../mm/vmm/vmm.h"
#include "../mm/handler/malloc.h"

/* KERNEL_HEAP_START and KERNEL_HEAP_MAX are defined in mm/handler/malloc.h */

void Initialize_memories(BootParams *bootParams);