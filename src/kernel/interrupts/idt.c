#include "idt.h"


idt_entry_32_t idt32[256];
idtr_32_t idtr32;


__attribute__ ((interrupt)) void default_excp_handler(int_frame_32_t *int_frame_32)
{
    printf("Default exception handler called without error code\n");
    // printf("Unhandled exception %d %s\n", regs->interrupt, g_Exceptions[regs->interrupt]);
        
    //     printf("  eax=%x  ebx=%x  ecx=%x  edx=%x  esi=%x  edi=%x\n",
    //            regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);

    //     printf("  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x\n",
    //            regs->esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds, regs->ss);

    //     printf("  interrupt=%x  errorcode=%x\n", regs->interrupt, regs->error);
}

__attribute__((interrupt)) void default_excp_handler_err_code(int_frame_32_t *int_frame_32, uint32_t error_code)
{
    printf("Default exception handler called with error code \n");
}

__attribute__((interrupt)) void  default_int_handler(int_frame_32_t *frame)
{
    printf("Default interrupt handler");
}

void set_idt_descriptor_32(uint8_t entry_number, void *isr, uint8_t flags)
{
    idt_entry_32_t *descriptor = &idt32[entry_number];

    descriptor->isr_address_low  = (uint32_t)isr & 0xFFFF;  // Low 16 bits of ISR address 
    descriptor->kernel_cs        = 0x08;                    // Kernel code segment containing this isr
    descriptor->reserved         = 0;                       // Reserved, must be set to 0
    descriptor->attributes       = flags;                   // Type & attributes (INT_GATE, TRAP_GATE, etc.)
    descriptor->isr_address_high = ((uint32_t)isr >> 16) & 0xFFFF;  // High 16 bits of ISR address 
}

void init_idt_32(void)
{
    idtr32.limit = (uint16_t)(sizeof idt32); // Size should be 8 bytes * 256 descriptors (0-255) 
    idtr32.base  = (uint32_t)&idt32; 

    // Set up exception handlers first (ISRs 0-31)
    for (uint8_t entry = 0; entry < 32; entry++) {
        if (entry == 8  || entry == 10 || entry == 11 || entry == 12 || 
            entry == 13 || entry == 14 || entry == 17 || entry == 21) {
            // Exception takes an error code
            set_idt_descriptor_32(entry, default_excp_handler_err_code, TRAP_GATE_FLAGS); // default exception handler for all the exceptions
        } else {
            // Exception does not take an error code
            set_idt_descriptor_32(entry, default_excp_handler, TRAP_GATE_FLAGS); // default exception handler for all the exceptions
        }
    }

    // Set up regular interrupts (ISRs 32-255)
    for (uint16_t entry = 32; entry < 256; entry++) {
        set_idt_descriptor_32(entry, default_int_handler, INT_GATE_FLAGS);
    }

    __asm__ __volatile__ ("lidt %0" : : "memory"(idtr32)); // Load IDT to IDT register

    set_idt_descriptor_32(0,div_by_0_handler,TRAP_GATE_FLAGS);
    set_idt_descriptor_32(0x80,syscall_dispatcher,INT_GATE_FLAGS);

    set_idt_descriptor_32(0x20, timer_irq0_handler, INT_GATE_FLAGS); 
    set_idt_descriptor_32(0x21, keyboard_irq1_handler, INT_GATE_FLAGS);
//    set_idt_descriptor_32(0x28, cmos_rtc_irq8_handler, INT_GATE_FLAGS);

    clear_irq_mask(0); // Enable timer (will tick every ~18.2/s)
    clear_irq_mask(1); // keyboard interrupt
    clear_irq_mask(2); // Enable PIC2 line
//    clear_irq_mask(8); // Enable CMOS RTC IRQ8


    //enable_rtc();

    __asm__ __volatile__("sti");

     uint16_t divisor = 100;
     uint16_t rate_generator_mode = 2;
     uint16_t channel = 0;

     set_pit_channel_mode_frequency(channel, rate_generator_mode, divisor);
      
}