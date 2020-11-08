
#include "paging.h"
#include "lib.h"

/*
 * init_paging
 * DESCRIPTION: initialize paging for the kernel
 * INPUTS: none
 * OUTPUTS: none
 * RETURNS: none
 * SIDE EFFECTS: initializes paging register bits, directory, and tables
 */
void init_paging()
{
    int i; /* looping variable */

    pde_4mb_t pde_4mb;       /* for init 4mb page directory entry */
    pde_pte_t pde_pte;       /* for init page directory entry     */
    pte_4kb_t pte_4kb;       /* for init page table               */

    /* init pde 4mb to link to 4mb pages */
    pde_4mb.present       = 0x0;
    pde_4mb.read_write    = 0x1;
    pde_4mb.supervisor    = 0x0;
    pde_4mb.write_through = 0x0;
    pde_4mb.cache_disable = 0x0;
    pde_4mb.accessed      = 0x0;
    pde_4mb.dirty         = 0x0;
    pde_4mb.page_size     = 0x1; /* Must be 1 because we're doing 4 mb pages */
    pde_4mb.global        = 0x0; /* TODO not sure about this */
    pde_4mb.ignored       = 0x1; /* doesn't really matter here */
    pde_4mb.mem_type      = 0x0; /* TODO not sure about this */
    pde_4mb.ptr_unused    = 0x0; /* prcsr can't suppport big addrs like this */
    pde_4mb.reserved      = 0x0; /* doesn't really matter here */
    pde_4mb.ptr           = 0x0; /* will be changed later */

    /* init pde table to link to tables */
    pde_pte.present       = 0x0;
    pde_pte.read_write    = 0x1;
    pde_pte.supervisor    = 0x0;
    pde_pte.write_through = 0x0;
    pde_pte.cache_disable = 0x0;
    pde_pte.accessed      = 0x0; /* not accessed yet */
    pde_pte.ignored0      = 0x0; /* doesn't matter */
    pde_pte.page_size     = 0x0; /* Must be 0 because we're doing 4kb pages */
    pde_pte.ignored1      = 0x0; /* doesn't matter */
    pde_pte.ptr           = 0x0; /* will be changed later */

    /* init entry into page directory for 4kb pages */
    pte_4kb.present       = 0x0;
    pte_4kb.read_write    = 0x1;
    pte_4kb.supervisor    = 0x0;
    pte_4kb.write_through = 0x0;
    pte_4kb.cache_disable = 0x0;
    pte_4kb.accessed      = 0x0;
    pte_4kb.dirty         = 0x0;
    pte_4kb.reserved      = 0x0; /* doesn't matter */
    pte_4kb.global        = 0x0; /* TODO check this */
    pte_4kb.ignored       = 0x0; /* doesn't matter */
    pte_4kb.ptr           = 0x0; /* will be changed later */

    /* init page directory (allocated in x86_desc.S) */
    for (i = 0; i < DIR_ENTRIES; i++) {
        /* initialize them all to table entries for now */
        page_dir[i] = pde_pte.val;
    }
    /* init page table (allocated in x86_desc.S) */
    for (i = 0; i < TABLE_ENTRIES; i++) {
        page_table[i] = pte_4kb.val;
    }

    /* init kernel page */
    pde_4mb.present   = 1;
    pde_4mb.page_size = 1;
    pde_4mb.ptr       = KERNEL_MEMORY_ADDR >> DIR_ENTRY_PAGE_OFFSET;
    page_dir[1] = pde_4mb.val; /* given to 1 index b/c that is found with    */
                               /* the desired memory access address 0x400000 */
    // map_v_p(KERNEL_MEMORY_ADDR, KERNEL_MEMORY_ADDR, 1);

    /* init video memory (and page directory entry for lowest 4 MB)*/
    pde_pte.present   = 1;
    pde_pte.page_size = 0;
    /* point the page directory entry to the 4-KB-page table */
    pde_pte.ptr       = ((uint32_t)&page_table[0]) >> TABLE_ENTRY_PAGE_OFFSET;
    page_dir[0]       = pde_pte.val;
    pte_4kb.present   = 1;
    /* map same virtual memory addr as physical memory addr */
    /* use only the bottom 20 bits of shifted video addr    */
    /*   Shouldn't need to because video addr is small,     */
    /*   but just to be technically correct.                */
    pte_4kb.ptr = 0xFFFFF & (VIDEO >> TABLE_ENTRY_PAGE_OFFSET);
    page_table[0xFFFFF & (VIDEO >> TABLE_ENTRY_PAGE_OFFSET)] = pte_4kb.val;
    // map_v_p(VIDEO, VIDEO, 0);

    /* we now init paging related registers, after we used physical */
    /* memory directly                                              */
    enable_paging();
}

void
enable_paging()
{
    asm volatile ("                                        \
        /* set cr3 to page directory location (31:12) */   \
        movl %0, %%eax                                    ;\
        andl $0xFFFFF000, %%eax                           ;\
        movl %%eax, %%cr3                                 ;\
                                                           \
        /* set cr4 bit 4 to allow mixed sized paging */    \
        movl %%cr4, %%eax                                 ;\
        orl $0x00000010, %%eax                            ;\
        movl %%eax, %%cr4                                 ;\
                                                           \
        /* set cr0 bit 31 and bit 0 to enable paging */    \
        movl %%cr0, %%eax                                 ;\
        orl $0x80000001, %%eax                            ;\
        movl %%eax, %%cr0                                 ;\
        "
        : /* no outputs */
        : "r"(&page_dir[0])
        : "eax"
    );
}

void
disable_paging()
{
    asm volatile ("                                        \
        /* unset cr0 bit 31 and bit 0 to disable paging */ \
        movl %%cr0, %%eax                                 ;\
        andl $0x7FFFFFFE, %%eax                            ;\
        movl %%eax, %%cr0                                ;\
        "
        : /* no outputs */
        : /* no inputs */
        : "eax"
    );
}

/* map_v_p
 * DESCRIPTION: Maps a virtual memory address location to a physical
 *                memory address location. If the physical address is
 *                0, then it unmaps the virtual address
 * INPUT: virtual_addr  -- virtual memory address
 *        physical_addr -- physical memory address, or 0 if unmapping
 *        kb_or_mb      -- whether it is an 8kb (0) or 4mb (1) page
 * OUTPUTS: none
 * RETURNS: -1 if fail, 0 if success
 * SIDE EFFECTS: updates page directory/tables
 */
uint32_t
map_v_p(uint32_t virtual_addr, uint32_t physical_addr, uint32_t kb_or_mb)
{
    /* Validate kb or mb option input */
    if (kb_or_mb != 0 && kb_or_mb != 1) return -1;

    /* Addresses cannot be 0 */
    if ((virtual_addr) == 0) return -1;

    /* Addresses must align correctly */
    if (kb_or_mb == 0) {
        if (((virtual_addr | physical_addr) & 0xFFF) != 0) return -1;
    } else {
        if (((virtual_addr | physical_addr) & 0x3FFF) != 0) return -1;
    }

    /* Memory address too large */
    // TODO implement

    /* If 8kb page, only allocation in the bottom 4mb is supported */
    if (kb_or_mb == 0 && virtual_addr >= 0x40000) return -1;

    pde_pte_t* pde_pte;
    pte_4kb_t* pde_4kb;
    pde_4mb_t* pde_4mb;       /* for 4mb page directory entry */

    if (kb_or_mb == 0) {
        /* Get page dir entry (Should always be 0 index of page_dir) */
        /*   ...2 because 2^2 bytes per entry                        */
        pde_pte = (pde_pte_t*)&page_dir[(virtual_addr >> DIR_ENTRY_PAGE_OFFSET)];

        pde_pte->page_size = 0;
        pde_pte->present = 1;    // Should always be present, but just to be sure

        /* Get page table entry */
        pde_4kb = (pte_4kb_t*)((uint32_t)(pde_pte->ptr << TABLE_ENTRY_PAGE_OFFSET)
                                + (virtual_addr & ((0x1 << DIR_ENTRY_PAGE_OFFSET) - 1)));

        /* map or unmap */
        if (physical_addr == 0) {
            pde_4kb->present = 0;
        } else {
            pde_4kb->present = 1;
            pde_4kb->ptr = physical_addr >> TABLE_ENTRY_PAGE_OFFSET;
        }
    } else {
        /* Get page dir entry                 */
        /*   ...2 because 2^2 bytes per entry */
        pde_4mb = (pde_4mb_t*)&page_dir[(virtual_addr >> DIR_ENTRY_PAGE_OFFSET)];

        pde_4mb->page_size = 1;

        /* map or unmap */
        if (physical_addr == 0) {
            pde_4mb->present = 0;
        } else {
            pde_4mb->present = 1;
            pde_4mb->ptr = physical_addr >> DIR_ENTRY_PAGE_OFFSET;
        }
    }
    return 0;
}
