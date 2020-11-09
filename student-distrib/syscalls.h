#ifndef _SYS_CALLS_H
#define _SYS_CALLS_H

#include "filesys.h"
#include "types.h"
#include "rtc.h"
#include "terminal.h"
#include "lib.h"

#define MIN_FD 					2
#define MAX_FD 					7
#define FILE_ARRAY_LEN 	8
#define MAX_DEVICES 		6

#define MB_8 						0x800000
#define KB_8            0x8000

#define ESP_MASK        0xFFFFE000
#define PCB_SIZE				0x2000 /* 8 kB pages */

/* File operations struct */
typedef struct {
	int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
	int32_t (*open) (const uint8_t* filename);
	int32_t (*close) (int32_t fd);
} fops_t;

/* Device struct */
typedef struct {
	uint8_t* name;
	fops_t* fops;
} dev_t;

/* File Descriptor Struct */
typedef struct {
    fops_t* fops;
    uint32_t inode;
    uint32_t pos;
    uint32_t flags;
} fd_t;

/* Process control block struct */
typedef struct pcb {
	fd_t file_array[FILE_ARRAY_LEN];
  int8_t arg_buffer[MAX_BUFF_LENGTH];
	int32_t p_id;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
} pcb_t;

/* Used for read/write/open/close */
pcb_t* get_current_PCB();

/* Will be used for halt/execute/scheduler */
pcb_t* find_PCB(int p_id);

int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);

/* get pointer to inode */
inode_t* get_inode_ptr(uint32_t inode);

/* Loads an executable file into correct location in memory */
int32_t load(dentry_t* d, uint8_t* mem);

/* Update process count */
int32_t add_process();
int32_t delete_process();

/* Assembly to return ESP register */
// #define get_esp(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%esp, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_ebp(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%ebp, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_esi(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%esi, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_edi(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%edi, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_eax(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%eax, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_ebx(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%ebx, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_ecx(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%ecx, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)
//
// #define get_edx(x)          \
// do {                        \
//     asm volatile (          \
//         "movl %%edx, %0"    \
//         : "=rm" (x)         \
//     );                      \
// } while(0)

#endif /* _SYS_CALLS_H */
