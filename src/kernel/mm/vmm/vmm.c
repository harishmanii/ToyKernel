#include "vmm.h"


page_directory *current_page_directory = 0;


page_directory* setup_page_directory_table()
{
    page_directory *dir = (page_directory *)allocate_page(3);
    if (!dir) return false; // Out of memory
     memset(dir, 0, sizeof(page_directory));

    for (uint32_t i = 0; i < 1024; i++)
        dir->entries[i] = 0x02; // Supervisor, read/write, not present

    return dir;
}

page_table* setup_page_table()
{
    page_table *table = (page_table *)allocate_page(1);
    if (!table) return false;   // Out of memory
    
    memset(table, 0, sizeof(page_table));

    for (uint32_t i = 0, frame = KERNEL_ADDRESS, virt = 0xC0000000; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        // Create new page
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        // Add page to 0-4MB page table
        table->entries[PT_INDEX(virt)] = page;
    }

    return table;
}

page_table* setup_higher_half_kernel()
{
    page_table *table3G = (page_table *)allocate_page(1);
    
    if (!table3G) return false;   // Out of memory

    memset(table3G, 0, sizeof(page_table));

    for (uint32_t i = 0, frame = 0x0, virt = 0x0; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        // Create new page
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        // Add page to 3GB page table
        table3G->entries[PT_INDEX(virt)] = page;
    }

    return table3G;
}

bool enable_paging()
{
 __asm__ __volatile__ ("movl %CR0, %EAX; orl $0x80000001, %EAX; movl %EAX, %CR0");

    return true;    
}

bool set_page_directory(page_directory *pd)
{
    if (!pd) return false;

    current_page_directory = pd;

    // CR3 (Control register 3) holds address of the current page directory
    __asm__ __volatile__ ("movl %%EAX, %%CR3" : : "a"(current_page_directory) );

    return true;
}

bool INITIALIZE_VMEMORY(void)
{
    page_directory *dir = setup_page_directory_table();

    page_table *table =  setup_page_table();
    
    page_table *table3G = setup_higher_half_kernel();

    pd_entry *entry = &dir->entries[PD_INDEX(0xC0000000)];
    SET_ATTRIBUTE(entry, PDE_PRESENT);
    SET_ATTRIBUTE(entry, PDE_READ_WRITE);
    SET_FRAME(entry, (physical_address)table); // 3GB directory entry points to default page table

    pd_entry *entry2 = &dir->entries[PD_INDEX(0x00000000)];
    SET_ATTRIBUTE(entry2, PDE_PRESENT);
    SET_ATTRIBUTE(entry2, PDE_READ_WRITE);   
    SET_FRAME(entry2, (physical_address)table3G);    // Default dir entry points to 3GB page table

    // Switch to page directory
    set_page_directory(dir);

    enable_paging();

}



