#pragma once

#include "exceptions.h"
#include "syscall.h"
#include "../include/stdio.h"

#define TRAP_GATE_FLAGS     0x8F    // P = 1, DPL = 00, S = 0, Type = 1111 (32bit trap gate)
#define INT_GATE_FLAGS      0x8E    // P = 1, DPL = 00, S = 0, Type = 1110 (32bit interrupt gate)
#define INT_GATE_USER_FLAGS 0xEE    // P = 1, DPL = 11, S = 0, Type = 1110 (32bit interrupt gate, called from PL 3)


typedef struct{
    uint16_t isr_address_low;   // Lower 16bits of isr address
    uint16_t kernel_cs;         // Code segment for this ISR
    uint8_t  reserved;          // Set to 0, reserved by intel
    uint8_t  attributes;        // Type and attributes; Flags
    uint16_t isr_address_high;  // Upper 16bits of isr address
} __attribute__ ((packed)) idt_entry_32_t;


typedef struct{
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed)) idtr_32_t;


extern idt_entry_32_t idt32[256];
extern idtr_32_t idtr32;





__attribute__ ((interrupt)) void default_excp_handler(int_frame_32_t *int_frame_32);
__attribute__((interrupt)) void default_excp_handler_err_code(int_frame_32_t *int_frame_32, uint32_t error_code);
__attribute__((interrupt)) void  default_int_handler(int_frame_32_t *frame);
void set_idt_descriptor_32(uint8_t entry_number, void *isr, uint8_t flags);
void init_idt_32(void);