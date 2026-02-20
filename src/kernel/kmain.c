
#include "boot/bootparams.h"
#include "hal/hal.h"
#include "include/stdio.h"
#include <stdint.h>
#include "include/time.h"

/*
    Refefernces are taken from OSwiki and brokenthorn
*/


void __attribute__((section(".entry"))) kstart(BootParams *bootParams) {
  HAL_Initialize(bootParams);
  while(1){
  printf("Enter your name : ");

  char* name = get();
  printf("\nYour name is %s\n",name);
  }


//   __asm__ __volatile__ (
//     "movl $0, %eax\n\t"
//     "int $0x80"
// );
// printf("s\n");
// sleep(3);
// printf("e\n");


  
  no_exit();
}
