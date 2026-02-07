#include "hal.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void HAL_Initialize(BootParams *bootParams) 
{
  memset(&__bss_start, 0, (&__end) - (&__bss_start));
  clrscr();
  i686_GDT_Initialize(); // initialize and add the entry in GDT
  i686_IDT_Initialize(); // init the IDT table
  i686_ISR_Initialize(); // init the entry with interrupts
  Initialize_memories(bootParams);
}

void no_exit()
{
  for(;;){
    __asm__ volatile ("hlt");
  }
}