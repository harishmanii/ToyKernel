#include "hal.h"



extern uint8_t __bss_start;
extern uint8_t __end;

void HAL_Initialize(BootParams *bootParams) 
{
  memset(&__bss_start, 0, (&__end) - (&__bss_start));
  clrscr();
  i686_GDT_Initialize(); // initialize and add the entry in GDT
  // Initialize_memories(bootParams); // need to consider
  init_idt_32();
  remap_pic();
}

void no_exit()
{
  for(;;){
    __asm__ volatile ("hlt");
  }
}