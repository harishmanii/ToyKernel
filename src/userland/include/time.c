#include "../lib/time.h"

void sleep(uint32_t seconds){
    sleepm(seconds*1000);
}

void sleepm(uint32_t milliseconds){
     __asm__ __volatile__("int $0x80" : : "a"(2), "b"((uint32_t)(milliseconds)) );
}