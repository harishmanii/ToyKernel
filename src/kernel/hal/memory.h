#pragma once
#include "../boot/bootparams.h"
#include "../mm/pmm/pmm.h"
#include "../mm/vmm/vmm.h"
#include "../mm/handler/malloc.h"

#define KERNEL_HEAP_START 0xD1000000
#define KERNEL_HEAP_MAX   0xC2000000

void Initialize_memories(BootParams *bootParams);