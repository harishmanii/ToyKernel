/*
 * src/kernel/loader/elf.h
 *
 * Minimal ELF32 structures and the runtime ELF loader used to map userland
 * executables into user address space.
 *
 * Only the fields / constants required for loading ET_EXEC ELF32 binaries
 * on i686 are defined here.  No section-header support – we walk program
 * headers only (the right tool for loading, not for debugging).
 */
#pragma once
#include <stdint.h>

/* ── ELF32 scalar types ──────────────────────────────────────────────────── */
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

/* ── ELF identification array ────────────────────────────────────────────── */
#define EI_NIDENT  16
#define EI_MAG0    0    /* magic byte 0 */
#define EI_MAG1    1    /* magic byte 1 */
#define EI_MAG2    2    /* magic byte 2 */
#define EI_MAG3    3    /* magic byte 3 */
#define EI_CLASS   4    /* file class   */
#define EI_DATA    5    /* data encoding */

#define ELFMAG0    0x7Fu
#define ELFMAG1    'E'
#define ELFMAG2    'L'
#define ELFMAG3    'F'

#define ELFCLASS32 1    /* 32-bit objects */
#define ELFDATA2LSB 1   /* little-endian  */

/* ── ELF file types (e_type) ─────────────────────────────────────────────── */
#define ET_EXEC  2      /* executable file */

/* ── Program-header segment types (p_type) ──────────────────────────────── */
#define PT_LOAD  1      /* loadable segment */

/* ── ELF32 file header ───────────────────────────────────────────────────── */
typedef struct {
    unsigned char e_ident[EI_NIDENT]; /* magic + class + data + ... */
    Elf32_Half    e_type;             /* object file type           */
    Elf32_Half    e_machine;          /* target ISA                 */
    Elf32_Word    e_version;          /* ELF version                */
    Elf32_Addr    e_entry;            /* virtual entry-point address */
    Elf32_Off     e_phoff;            /* byte offset of program-header table */
    Elf32_Off     e_shoff;            /* byte offset of section-header table */
    Elf32_Word    e_flags;            /* processor-specific flags   */
    Elf32_Half    e_ehsize;           /* ELF header size in bytes   */
    Elf32_Half    e_phentsize;        /* program-header entry size  */
    Elf32_Half    e_phnum;            /* number of program-header entries */
    Elf32_Half    e_shentsize;        /* section-header entry size  */
    Elf32_Half    e_shnum;            /* number of section-header entries */
    Elf32_Half    e_shstrndx;         /* section-name string table index */
} Elf32_Ehdr;

/* ── ELF32 program header ────────────────────────────────────────────────── */
typedef struct {
    Elf32_Word p_type;   /* segment type                          */
    Elf32_Off  p_offset; /* byte offset of segment in file        */
    Elf32_Addr p_vaddr;  /* virtual address of segment in memory  */
    Elf32_Addr p_paddr;  /* physical address (ignored on x86)     */
    Elf32_Word p_filesz; /* number of bytes in the file image     */
    Elf32_Word p_memsz;  /* number of bytes in the memory image   */
    Elf32_Word p_flags;  /* segment flags (R/W/X)                 */
    Elf32_Word p_align;  /* alignment                             */
} Elf32_Phdr;

/* ── Return codes from elf_load ──────────────────────────────────────────── */
#define ELF_OK           0
#define ELF_ERR_MAGIC   -1   /* bad 0x7F ELF magic                */
#define ELF_ERR_CLASS   -2   /* not ELFCLASS32                    */
#define ELF_ERR_TYPE    -3   /* not ET_EXEC                       */
#define ELF_ERR_NO_LOAD -4   /* no PT_LOAD segment found          */
#define ELF_ERR_OOM     -5   /* physical-page allocator returned 0 */

/*
 * elf_load  –  parse an ELF32 executable and map its PT_LOAD segments.
 *
 *   data      – pointer to the raw ELF image in kernel-accessible memory
 *   entry_out – receives the virtual entry-point address (e_entry)
 *
 * Each PT_LOAD segment is:
 *   1. Allocated page-by-page from the PMM (pages are pre-zeroed).
 *   2. Filled with file data for the range [0, p_filesz); the remainder
 *      (the BSS region) stays zero.
 *   3. Mapped at p_vaddr with ring-3 (user) read/write/execute permissions
 *      via map_page_user().
 *
 * Returns ELF_OK (0) on success or a negative ELF_ERR_* code on failure.
 */
int elf_load(const void *data, uint32_t *entry_out);
