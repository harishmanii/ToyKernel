
#include <stdint.h>
#include "include/stdio.h"
#include "hal/hal.h"



void __attribute__((section(".entry"))) kstart(uint16_t bootDrive)
{
    
    HAL_Initialize();
    printf("Hello world from kernel!!!\n");
    //int a = 1/0;

end:
    for (;;);
}
