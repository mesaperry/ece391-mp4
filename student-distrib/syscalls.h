#ifndef _SYS_CALLS_H
#define _SYS_CALLS_H
extern void jump_userspace(void);

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
#define MB_128          0x8000000
#define MB_4            0x400000
#define KB_8            0x2000

#define USER_PROCESS_START_PHYSICAL   MB_8
#define USER_PROCESS_START_VIRTUAL    MB_128
#define USER_PROCESS_IMAGE_OFFSET     0x48000
#define USER_PROCESS_SIZE             MB_4
#define USER_PROCESS_STACK            USER_PROCESS_START_VIRTUAL + USER_PROCESS_SIZE - 0x4
#define ELF_OFFSET 										24
#define USER_VIDMAP							0x4000000

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
	uint8_t file_name[FNAME_MAX_LEN];
} fd_t;

/* Process control block struct */
typedef struct pcb {
	fd_t file_array[FILE_ARRAY_LEN];
 	uint8_t arg_buffer[MAX_BUFF_LENGTH];
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

/* Checkpoint 4 syscalls */
// int32_t getargs (uint8_t* buf, uint32_t nbytes);
// int32_t vidmap (uint8_t** screen_start);
// int32_t set_handler (int32_t signum, void* handler_address);
// int32_t sigreturn (void);

/* Loads an executable file into correct location in memory */
int32_t load(dentry_t* d, uint8_t* mem);

/* Update process count */
int32_t add_process();
int32_t delete_process();

#endif /* _SYS_CALLS_H */
