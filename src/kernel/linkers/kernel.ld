ENTRY(kstart)
OUTPUT_FORMAT("elf32-i386")

phys = 0x00100000;
virt = 0xC0000000;

SECTIONS
{
    /* Move location counter to higher half */
    . = virt;
    __kernel__start = .;

    .text : AT(phys)
    {
        __entry_start = .;
        *(.entry)

        __text_start = .;
        *(.text)
    }

    .data : AT(phys + (__data_start - virt))
    {
        __data_start = .;
        *(.data*)
    }

    .rodata : AT(phys + (__rodata_start - virt))
    {
        __rodata_start = .;
        *(.rodata*)
    }

    .bss : AT(phys + (__bss_start - virt))
    {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
    }

    __end = .;
    __kernel__end = .;
}
