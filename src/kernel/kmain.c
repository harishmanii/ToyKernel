
#include <stdint.h>
#include "include/stdio.h"
#include "hal/hal.h"
#include "boot/bootparams.h"



void __attribute__((section(".entry"))) kstart(BootParams* bootParams)
{
    
    HAL_Initialize();
    printf("Hello world from kernel!!!\n");
    //int a = 1/0;
    for (int i = 0; i < bootParams->Memory.RegionCount; i++) 
    {
/*
bootParams->Memory.Regions[i].Begin,
            bootParams->Memory.Regions[i].Length,
            bootParams->Memory.Regions[i].Type);
*/
printf("MEM: start=0x%llx length=0x%llx type=%x \n",bootParams->Memory.Regions[i].Begin,
bootParams->Memory.Regions[i].Length,bootParams->Memory.Regions[i].Type);
printf("\n----------------------------------\n");
    }

end:
    for (;;);
}
