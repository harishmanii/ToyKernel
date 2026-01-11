
#include <stdint.h>
#include "include/stdio.h"
#include "include/string.h"
#include "include/hal.h"

extern uint8_t __bss_start;
extern uint8_t __end;

extern char __kernel__start;
extern char __kernel__end;

void __attribute__((section(".entry"))) kstart(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();
    HAL_Initialize();

    printf("Hello world from kernel!!!\n");
    int a = 1/0;

end:
    for (;;);
}
