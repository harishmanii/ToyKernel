#pragma once
#include "../boot/bootparams.h"
#include "../arch/i686/gdt.h"
#include "../arch/i686/idt.h"
#include "../arch/i686/isr.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../mm/pmm.h"

void HAL_Initialize(BootParams* bootParams);