#include "boot/bootparams.h"
#include "hal/hal.h"

void kstart(BootParams *bootParams)
{
    HAL_Initialize(bootParams);
    no_exit();
}
