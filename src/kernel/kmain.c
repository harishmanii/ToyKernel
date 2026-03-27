
#include "boot/bootparams.h"
#include "hal/hal.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include <stdint.h>
#include "include/time.h"

/*
    Refefernces are taken from OSwiki,brokenthorn,nanobyte YT and Queso Fuego YT
*/


void kstart(BootParams *bootParams) {
  HAL_Initialize(bootParams);
  
  printf("Hello world\n");
  no_exit();
  
}
