#include "vmm.h"


page_directory *current_page_directory = 0;


//setting up the initial page directory which holds the page directory for the mmu it needed
// initially it setup with supervisor and read/write mode only without present bit ,if we use the page na we can add some attribute
// so thats why we allocate a single page directory
bool setup_page_directory_table()
{
    current_page_directory = (page_directory *)allocate_block(1);
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
    page_table *table = (page_table *)allocate_block(1);
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
    page_table *table = (page_table *)allocate_block(1);
    
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

pt_entry *get_page(const virtual_address address)
{
    // Get page directory
    page_directory *pd = current_page_directory; 

    // Get page table in directory
    pd_entry   *entry = &pd->entries[PD_INDEX(address)];
    page_table *table = (page_table *)PAGE_PHYS_ADDRESS(entry);

    // Get page in table
    pt_entry *page = &table->entries[PT_INDEX(address)];
    
    // Return page
    return page;
}

void* allocate_page(pt_entry *page)
{
    void *block = (void *)allocate_block(1);

    if (block){
        SET_FRAME(page,(physical_address)block);
        SET_ATTRIBUTE(page,PTE_PRESENT);
    }
    return block;
}

void flush_tlb_entry(virtual_address address)
{
    __asm__ __volatile__ ("cli; invlpg (%0); sti" : : "r"(address) );
}

void free_page(pt_entry *page)
{
    void *address = (void *)PAGE_PHYS_ADDRESS(page);
    if(address) deallocate_memory_by_size((uint32_t)address,1);

    CLEAR_ATTRIBUTE(page,PTE_PRESENT);
}

void unmap_page(void *virt_address)
{
    pt_entry *page = get_page((uint32_t)virt_address);

    SET_FRAME(page, 0);     // Set physical address to 0 (effectively this is now a null pointer)
    CLEAR_ATTRIBUTE(page, PTE_PRESENT); // Set as not present, will trigger a #PF
}

//If the virtual address was already mapped it will overwrite
bool map_page(void *phys_address,void *virt_address )
{
    // Get page
    page_directory *pd = current_page_directory;

    // Get page table
    pd_entry *entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

    // TODO: Use TEST_ATTRIBUTE for this check?
    if ((*entry & PTE_PRESENT) != PTE_PRESENT) {
        // Page table not present allocate it
        page_table *table = (page_table *)allocate_block(1);
        if (!table) return false;   // Out of memory

        // Clear page table
        memset(table, 0, sizeof(page_table));

        // Create new entry
        pd_entry *entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

        // Map in the table & enable attributes
        SET_ATTRIBUTE(entry, PDE_PRESENT);
        SET_ATTRIBUTE(entry, PDE_READ_WRITE);
        SET_FRAME(entry, (physical_address)table);
    }

    // Get table 
    page_table *table = (page_table *)PAGE_PHYS_ADDRESS(entry);

    // Get page in table
    pt_entry *page = &table->entries[PT_INDEX((uint32_t)virt_address)];

    // Map in page
    SET_ATTRIBUTE(page, PTE_PRESENT);
    SET_FRAME(page, (uint32_t)phys_address);

    return true;
}


bool map_page_user(void *phys_address, void *virt_address)
{
    page_directory *pd = current_page_directory;

    pd_entry *entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

    if ((*entry & PDE_PRESENT) != PDE_PRESENT) {
        /* No page table yet – allocate and initialise one. */
        page_table *table = (page_table *)allocate_block(1);
        if (!table) return false;
        memset(table, 0, sizeof(page_table));

        pd_entry *e = &pd->entries[PD_INDEX((uint32_t)virt_address)];
        SET_ATTRIBUTE(e, PDE_PRESENT);
        SET_ATTRIBUTE(e, PDE_READ_WRITE);
        SET_ATTRIBUTE(e, PDE_USER);          /* allow ring-3 access to this table */
        SET_FRAME(e, (physical_address)table);
    } else {
        /* Page table already exists – ensure the USER bit is set on the PDE. */
        SET_ATTRIBUTE(entry, PDE_USER);
    }

    /* Re-read entry after possible modification above. */
    entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];
    page_table *table = (page_table *)PAGE_PHYS_ADDRESS(entry);

    pt_entry *page = &table->entries[PT_INDEX((uint32_t)virt_address)];
    SET_ATTRIBUTE(page, PTE_PRESENT);
    SET_ATTRIBUTE(page, PTE_READ_WRITE);
    SET_ATTRIBUTE(page, PTE_USER);           /* allow ring-3 access to this page */
    SET_FRAME(page, (uint32_t)phys_address);

    flush_tlb_entry((virtual_address)virt_address);
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

    return true;
}



