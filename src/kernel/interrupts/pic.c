#include "pic.h"

/*
    Refefernces are taken from OSwiki and brokenthorn
*/


uint32_t *sleep_timer_ticks = (uint32_t *)IRQ0_SLEEP_TIMER_TICKS_AREA;

// Send end of interrupt command to signal IRQ has been handled
void send_pic_eoi(uint8_t irq)
{
    // slave port
    if (irq >= 8) outb(PIC_2_CMD, PIC_EOI);

    // master port
    outb(PIC_1_CMD, PIC_EOI);
}

// Disable PIC, if using APIC or IOAPIC
void disable_pic(void)
{
    outb(PIC_2_DATA, 0xFF);
    outb(PIC_1_DATA, 0xFF);
}

// Set IRQ mask by setting the bit in the IMR (interrupt mask register)
//   This will ignore the IRQ
void set_irq_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8) port = PIC_1_DATA;
    else {
        irq -= 8;
        port = PIC_2_DATA;
    }    
    //   to IMR
    // NOTE: Masking IRQ2 will stop 2nd PIC from raising IRQs due to 2nd PIC being
    //   mapped to IRQ2 in PIC1
    value = inb(port) | (1 << irq); // Get current IMR value, set on the IRQ bit to mask it, then write new value
    outb(port, value);
}

// Clear IRQ mask by clearing the bit in the IMR (interrupt mask register)
//   This will enable recognizing the IRQ
void clear_irq_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8) port = PIC_1_DATA;
    else {
        irq -= 8;
        port = PIC_2_DATA;
    }

    // Get current IMR value, clear the IRQ bit to unmask it, then write new value
    //   to IMR
    // NOTE: Clearing IRQ2 will enable the 2nd PIC to raise IRQs due to 2nd PIC being
    //   mapped to IRQ2 in PIC1
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// Remap PIC to use interrupts above first 15, to not interfere with exceptions (ISRs 0-31)
void remap_pic(void)
{
    uint8_t pic_1_mask, pic_2_mask;

    // Save current masks
    pic_1_mask = inb(PIC_1_DATA);
    pic_2_mask = inb(PIC_2_DATA);

    // ICW 1 (Initialization control word) - bit 0 = send up to ICW 4, bit 4 = initialize PIC
    // reset , clear ISR/IRR and ready to use cascade mode , Prepare to receive ICW2, ICW3, ICW4 in that exact order
    outb(PIC_1_CMD, 0x11);
    io_wait();
    outb(PIC_2_CMD, 0x11);
    io_wait();

    // ICW 2 - Where to map the base interrupt in the IDT
    outb(PIC_1_DATA, NEW_IRQ_0);
    io_wait();
    outb(PIC_2_DATA, NEW_IRQ_8);
    io_wait();
    
    // ICW 3 - Where to map PIC2 to the IRQ line in PIC1; Mapping PIC2 to IRQ 2
    outb(PIC_1_DATA, 0x4);  // Bit # (0-based) - 0100 = bit 2 = IRQ2
    io_wait();
    outb(PIC_2_DATA, 0x2);  // Binary # for IRQ in PIC1, 0010 = 2
    io_wait();
   
    // ICW 4 - Set x86 mode
    outb(PIC_1_DATA, 0x1); 
    io_wait();
    outb(PIC_2_DATA, 0x1); 
    io_wait();

    // Save current masks
    outb(PIC_1_DATA, pic_1_mask);
    outb(PIC_2_DATA, pic_2_mask);
}


__attribute__ ((interrupt)) void timer_irq0_handler(int_frame_32_t *frame)
{
    if (*sleep_timer_ticks > 0) (*sleep_timer_ticks)--;

    send_pic_eoi(0);
}

void set_pit_channel_mode_frequency(const uint8_t channel, const uint8_t operating_mode, const uint16_t divisor)
{
    // Invalid input
    if (channel > 2) return;

    __asm__ __volatile__ ("cli");

    /* PIT I/O Ports:
     * 0x40 - channel 0     (read/write) 
     * 0x41 - channel 1     (read/write) 
     * 0x42 - channel 2     (read/write) 
     * 0x43 - Mode/Command  (write only) 
     *
     * 0x43 Command register value bits (1 byte):
     * 7-6 select channel:
     *      00 = channel 0
     *      01 = channel 1
     *      10 = channel 2
     *      11 = read-back command
     * 5-4 access mode:
     *      00 = latch count value
     *      01 = lobyte only
     *      10 = hibyte only
     *      11 = lobyte & hibyte
     * 3-1 operating mode:
     *      000 = mode 0 (interrupt on terminal count)
     *      001 = mode 1 (hardware re-triggerable one-shot)
     *      010 = mode 2 (rate generator)
     *      011 = mode 3 (square wave generator)
     *      100 = mode 4 (software triggered strobe)
     *      101 = mode 5 (hardware triggered strobe)
     *      110 = mode 6 (rate generator, same as 010)
     *      111 = mode 7 (square wave generator, same as 011)
     * 0  BCD/Binary mode:
     *      0 = 16bit binary
     *      1 = 4-digit BCD (x86 does not use this!)
     */

    // Send the command byte, always sending lobyte/hibyte for access mode
    outb(0x43, (channel << 6) | (0x3 << 4) | (operating_mode << 1));

    // Send the frequency divider to the input channel , here we can send 8 bit at a time so we send the first part then second part
    outb(0x40 + channel, (uint8_t)divisor);           // Low byte
    outb(0x40 + channel, (uint8_t)(divisor >> 8));    // High byte

    __asm__ __volatile__ ("sti");
}


// Keyboard IRQ1 handler , implmeneted as per PS2 keyyboard documentation and get the refernce from oswikki
// need to enchance when we needed this
__attribute__ ((interrupt)) void keyboard_irq1_handler(int_frame_32_t *frame)
{
    //Make Key press
    //Break Key release
    enum {
        LSHIFT_MAKE  = 0x2A,
        LSHIFT_BREAK = 0xAA,
        RSHIFT_MAKE  = 0x36,
        RSHIFT_BREAK = 0xB6,
        LCTRL_MAKE   = 0x1D,
        LCTRL_BREAK  = 0x9D,
    };

    uint8_t key;

    // TODO: Add keyboard initialization & scancode functions, 
    //   do not assume scancode set 1
    
    // Scancode set 1 -> Ascii lookup table
    const uint8_t *scancode_to_ascii = "\x00\x1B" "1234567890-=" "\x08"
    "\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
    "zxcvbnm,./" "\x00\x00\x00" " ";

    // Shift key pressed on number row lookup table (0-9 keys)
    const uint8_t *num_row_shifts = ")!@#$%^&*(";

    // Set current key to null
    key_info->key = 0;

    key = inb(PS2_DATA_PORT);   // Read in new key

    if (key) {
        if      (key == LSHIFT_MAKE  || key == RSHIFT_MAKE) key_info->shift = true; 
        else if (key == LSHIFT_BREAK || key == RSHIFT_BREAK) key_info->shift = false; 
        else if (key == LCTRL_MAKE)  key_info->ctrl = true;
        else if (key == LCTRL_BREAK) key_info->ctrl = false;

        else {
            if (!(key & 0x80)) {
                // Only handle key presses.
                // Don't translate escaped scancodes, only return them
                key = scancode_to_ascii[key]; 

                    // If pressed shift, translate key to shifted key
                    if (key_info->shift) {
                        if      (key >= 'a' && key <= 'z') key -= 0x20;  // Convert lowercase into uppercase
                        else if (key >= '0' && key <= '9') key = num_row_shifts[key-0x30];  // Get int value of character, offset into shifted nums
                        else {
                            if      (key == '=')  key = '+';
                            else if (key == '\\') key = '|'; 
                            else if (key == '`')  key = '~';
                            else if (key == '[')  key = '{';
                            else if (key == ']')  key = '}';
                            else if (key == '\'') key = '\"';
                            else if (key == ';')  key = ':';
                            else if (key == ',')  key = '<';
                            else if (key == '.')  key = '>';
                            else if (key == '/')  key = '?';
                            // TODO: Add more shifted keys here...
                        }
                    }
                
                key_info->key = key;    // Set ascii key value in struct
            }
        }
    }
    send_pic_eoi(1);
}
