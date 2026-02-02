#include "vmm.h"


page_directory *current_page_directory = 0;


//setting up the initial page directory which holds the page directory for the mmu it needed
// initially it setup with supervisor and read/write mode only without present bit ,if we use the page na we can add some attribute
// so thats why we allocate a single page directory
bool setup_page_directory_table()
{
    current_page_directory = (page_directory *)allocate_page(1);
    if (!current_page_directory) return false; // Out of memory
     memset(current_page_directory, 0, sizeof(page_directory));

    for (uint32_t i = 0; i < 1024; i++)
        current_page_directory->entries[i] = 0x0;

    return true;
}

// for initially we need a single page table for enabling the paging feature after that we can get as we need
// 1024 is 1024 entries each page table can hold 1024 entries  
// here we creating page table for setting up the higher half addr
// here we ,map our kernel physical address with virtual address  
bool setup_higher_half_kernel()
{
    page_table *table = (page_table *)allocate_page(1);
    if (!table) return false;   // Out of memory
    
    memset(table, 0, sizeof(page_table));

    for (uint32_t i = 0, frame = KERNEL_ADDRESS, virt = 0xC0000000; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        // Add page to 0-4MB page table
        table->entries[PT_INDEX(virt)] = page;
    }
    // setting up the higher half address with our page we created above with the page_directory entry
    pd_entry *entry = &current_page_directory->entries[PD_INDEX(0xC0000000)];
    SET_ATTRIBUTE(entry, PDE_PRESENT);
    SET_ATTRIBUTE(entry, PDE_READ_WRITE);
    SET_FRAME(entry, (physical_address)table); // 3GB directory entry points to default page table

    return true;
}

// here we map this as identity mapping so kernel can jmp to this without page fault error
// here we just copy a current physical addr as virtual address so when kernel enable the paging it will pass the addr to mmu , mmu will check for the page directory but will not there a entry so it will throws page fault exception
// here also we map the page table with page directory
bool setup_page_table()
{
    page_table *table = (page_table *)allocate_page(1);
    
    if (!table) return false;   // Out of memory

    memset(table, 0, sizeof(page_table));

    for (uint32_t i = 0, frame = 0x0, virt = 0x0; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        table->entries[PT_INDEX(virt)] = page;
    }
    pd_entry *entry2 = &current_page_directory->entries[PD_INDEX(0x00000000)];
    SET_ATTRIBUTE(entry2, PDE_PRESENT);
    SET_ATTRIBUTE(entry2, PDE_READ_WRITE);   
    SET_FRAME(entry2, (physical_address)table);

    return true;

}

bool enable_paging()
{
    __asm__ __volatile__ ("movl %CR0, %EAX; orl $0x80000001, %EAX; movl %EAX, %CR0");

    return true;    
}

bool set_page_directory()
{
    if (!current_page_directory) return false;

    // CR3 (Control register 3) holds address of the current page directory
    __asm__ __volatile__ ("movl %%EAX, %%CR3" : : "a"(current_page_directory) );

    return true;
}

bool INITIALIZE_VMEMORY(void)
{
    setup_page_directory_table();

    setup_higher_half_kernel();

    setup_page_table();
    
    set_page_directory();

    // after enabled the paging we are still in the identity mapping , need to move to higher half 
    enable_paging();

    return true
}



