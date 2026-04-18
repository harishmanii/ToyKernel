#pragma once
#include "../arch/i686/gdt.h"
#include "../arch/i686/tss.h"
#include "../arch/i686/idt.h"
#include "../arch/i686/isr.h"
#include "../boot/bootparams.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../interrupts/idt.h"
#include "../interrupts/pic.h"
#include "../scheduler/task.h"

#include "memory.h"

void HAL_Initialize(BootParams *bootParams);
void no_exit();