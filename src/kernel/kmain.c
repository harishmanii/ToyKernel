#include "boot/bootparams.h"
#include "hal/hal.h"
#include "include/stdio.h"
#include <stdint.h>

void kstart(BootParams *bootParams)
{
    HAL_Initialize(bootParams);
    no_exit();
}
