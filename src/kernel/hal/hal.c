#include "../include/hal.h"
#include "../arch/i686/gdt.h"
#include "../arch/i686/idt.h"
#include "../arch/i686/isr.h"

void HAL_Initialize()
{
    i686_GDT_Initialize();  // initialize and add the entry in GDT
    i686_IDT_Initialize();  // init the IDT table
    i686_ISR_Initialize();  // init the entry with interrupts
}