#include "hal.h"
#include "../arch/i686/gdt.h"
#include "../arch/i686/idt.h"
#include "../arch/i686/isr.h"
#include "../include/string.h"
#include "../include/stdio.h"

extern uint8_t __bss_start;
extern uint8_t __end;

extern char __kernel__start;
extern char __kernel__end;




void HAL_Initialize()
{
    
    //bss initialization 
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    i686_GDT_Initialize();  // initialize and add the entry in GDT
    i686_IDT_Initialize();  // init the IDT table
    i686_ISR_Initialize();  // init the entry with interrupts

    clrscr();
}