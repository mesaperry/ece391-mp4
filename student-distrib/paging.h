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

/* Initialize paging in kernel */
extern void init_paging();

/* Translate virtual to physical memory location - 4 KB pages */
extern uint32_t trnsl_v_to_p(uint32_t p);

#endif
