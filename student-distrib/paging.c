
#include "paging.h"
#include "lib.h"

static void init_paging_registers();

void init_paging()
{
    int i; /* looping variable */

    pde_t pde;     /* for init page directory */
    pte_t pte;        /* for init page table     */

    /* init pde */
    pde.present = 0;
    pde.page_size = 0;

    /* init pte */
    pte.present = 0;

    /* init page directory (allocated in x86_desc.S) */
    for (i = 0; i < DIR_ENTRIES; i++) {
        page_dir[i] = pde;
    }
    /* init page table (allocated in x86_desc.S) */
    for (i = 0; i < TABLE_ENTRIES; i++) {
        page_table[i] = pte;
    }

    // /* init first entry in page directory to table */
    // pde.present = 1;
    // pde.pagetable_addr = page_table >> TABLE_ENTRY_PAGE_OFFSET;
    // page_dir[0] = pde_kb;

    /* init kernel page */
    pde.present   = 1;
    pde.page_size = 1;
    pde.ptr       = KERNEL_MEMORY_ADDR >> TABLE_ENTRY_PAGE_OFFSET;
    page_dir[1] = pde;    /* given to 1 index b/c that is found with    */
                          /* the desired memory access address 0x400000 */

    /* init video memory (and page directory entry for lowest 4 MB)*/
    pde.present   = 1;
    pde.page_size = 0;
    /* point the page directory entry to the 4-KB-page table */
    pde.ptr       = ((uint32_t)&page_table[0]) >> TABLE_ENTRY_PAGE_OFFSET;
    page_dir[0] = pde;
    pte.present = 1;
    pte.ptr = VIDEO >> TABLE_ENTRY_PAGE_OFFSET;
    page_table[VIDEO >> TABLE_ENTRY_PAGE_OFFSET] = pte;

    /* we now init paging related registers, after we used physical */
    /* memory directly                                              */
    init_paging_registers();

}

void
init_paging_registers()
{
    asm volatile ("                                        \
        /* set cr0 bit 31 to enable paging */             ;\
        movl %%cr0, %%eax                                 ;\
        orl $0x40000000, %%eax                            ;\
        movl %%eax, %%cr0                                 ;\
                                                          ;\
        /* set cr3 to page directory location */          ;\
        movl %0, %%cr3                                    ;\
                                                          ;\
        /* set cr4 bit 4 to allow mixed sized paging */   ;\
        movl %%cr4, %%eax                                 ;\
        orl $0x00000010, %%eax                            ;\
        movl %%eax, %%cr4                                 ;\
        "
        :
        : "r"(page_dir)
        : "eax"
    );
}

/* trnsl_v_to_p
 * DESCRIPTION: Translate virtual memory address
                to physical memory address
 * INPUTS: virtual memory address
 * OUTPUTS: none
 * RETURNS: physical memory address,
 * SIDE EFFECTS: none
 */
uint32_t trnsl_v_to_p(uint32_t p)
{
    if (!p) return -1;  /* null check */

    /* get pointer to page directory entry */
    uint32_t page_directory_entry_ptr = (uint32_t)&page_dir[0] + (p >> DIR_ENTRY_PAGE_OFFSET);
    /* check entry is present */
    if (!(*(pde_t*)page_directory_entry_ptr).present) return -1;

    /* check if entry is 4mb or 4kb */
    if ((*(pde_t*)page_directory_entry_ptr).page_size) {
        /* its a 4kb entry */
        uint32_t page_table_entry_ptr = (*(uint32_t*)page_directory_entry_ptr)
                + ((p << (32 - DIR_ENTRY_PAGE_OFFSET))
                    >> TABLE_ENTRY_PAGE_OFFSET);
        return (*(uint32_t*)page_table_entry_ptr)           /* page_ptr    */
                + ((p << (32 - TABLE_ENTRY_PAGE_OFFSET)) /* plus offset */
                      >> (32 - TABLE_ENTRY_PAGE_OFFSET));
    } else {
        /* its a 4mb entry */
        return (*(uint32_t*)page_directory_entry_ptr)
                + ((p << (32 - DIR_ENTRY_PAGE_OFFSET))
                      >> (32 - DIR_ENTRY_PAGE_OFFSET));
    }
}
