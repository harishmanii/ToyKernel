
#include "boot/bootparams.h"
#include "hal/hal.h"
#include "include/stdio.h"
#include <stdint.h>

void __attribute__((section(".entry"))) kstart(BootParams *bootParams) {
  HAL_Initialize(bootParams);
  printf("Hello world from kernel!!!\n");

  __asm__ __volatile__ (
    "movl $0, %eax\n\t"
    "int $0x80"
);



  
  no_exit();
}
