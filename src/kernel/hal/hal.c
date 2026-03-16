#include "hal.h"



extern uint8_t __bss_start;
extern uint8_t __end;

// void task1(){
//  while(1){
//     printf("task - 1\n");
   
//   }
// }

// void task2(){
 
//   while(1){
//     printf("task - 2\n");
//   }
// }


void HAL_Initialize(BootParams *bootParams) 
{
  memset(&__bss_start, 0, (&__end) - (&__bss_start));
  clrscr();
  i686_GDT_Initialize(); // initialize and add the entry in GDT
  Initialize_memories(bootParams); // need to consider
  remap_pic();
  init_idt_32();
  init_task();
  // create_task(task1);
  // create_task(task2);
  
}

void no_exit()
{
  for(;;){
    __asm__ volatile ("hlt");
  }
}