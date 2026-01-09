
#include <stdint.h>
#include "include/stdio.h"
#include "include/string.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void __attribute__((section(".entry"))) kstart(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    clrscr();

   printf("Hello world from kernel!!!\n");

end:
    for (;;);
}
