#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ASMCALL __attribute__((cdecl))

void ASMCALL x86_outb(uint16_t port, uint8_t data);

#define BOCHS_BREAKPOINT    __asm("xchgw %bx, %bx")


uint8_t ASMCALL x86_inb(uint16_t port);
