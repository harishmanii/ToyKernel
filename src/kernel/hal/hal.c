#include "hal.h"



extern uint8_t __bss_start;
extern uint8_t __end;

void task1(){
 while(1){
    printf("task - 1\n");
   
  }
}

void task2(){
 
  while(1){
    printf("task - 2\n");
  }
}


static void user_program(void)
{
    for (;;){
        __asm__ volatile ("int $0x80" : : "a"(1) : "memory"); /* syscall 1 */
      //  printf("tested\n"); //dont use the printf here because it is kernel code if run this on user program can trigger the exception
    }
}


void HAL_Initialize(BootParams *bootParams) 
{
  memset(&__bss_start, 0, (&__end) - (&__bss_start));
  clrscr();
  i686_GDT_Initialize(); // initialize and add the entry in GDT
  tss_init();             // patch GDT[5] and load the task register
  Initialize_memories(bootParams); // need to consider
  remap_pic();
  init_idt_32();
  init_task();
  //create_task(task1);
  //create_task(task2);
  //create_user_task(user_program);
  
}

void no_exit()
{
  for(;;){
    __asm__ volatile ("hlt");
  }
}