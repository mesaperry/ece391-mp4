/* paging.h
 *
 * Paging util file for initialization
 * and virtual memory manipulation
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "x86_desc.h"

#define TABLE_ENTRY_PAGE_OFFSET 12 /* byte offset required to get table entry */
#define DIR_ENTRY_PAGE_OFFSET 22   /* byte offset required to get dir entry */
#define MASK_10_BIT 0x3FF          /* mask for getting just the bottom 10 bits */

/* Initialize paging in kernel */
void init_paging();
/* Enable paging in control registers */
void enable_paging();
/* Disable paging in control registers */
void disable_paging();
/* Maps virtual memory to physical memory */
uint32_t map_v_p(uint32_t virtual_addr, uint32_t physical_addr,
    uint32_t kb_or_mb,
    uint32_t read_write,
    uint32_t user_supervisor);
void flush_tlb();
#endif
