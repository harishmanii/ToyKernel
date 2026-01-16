
#include <stdint.h>
#include "include/stdio.h"
#include "hal/hal.h"
#include "boot/bootparams.h"



void __attribute__((section(".entry"))) kstart(BootParams* bootParams)
{
    HAL_Initialize(bootParams);  
    printf("Hello world from kernel!!!\n");
}
