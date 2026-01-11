ENTRY(kstart)
OUTPUT_FORMAT("elf32-i386")
phys = 0x00100000;

SECTIONS
{
    . = phys;
    __kernel__start = .;
    .entry              : { __entry_start = .;      *(.entry)   }
    .text               : { __text_start = .;       *(.text)    }
    .data               : { __data_start = .;       *(.data)    }
    .rodata             : { __rodata_start = .;     *(.rodata)  }
    .bss                : { __bss_start = .;        *(.bss)     }
    __end = .;
    __kernel__end = .;
}