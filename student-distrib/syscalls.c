/* Created by Team 11 */
#include "syscalls.h"
#include "paging.h"
#include "lib.h"
#include "x86_desc.h"
#include "terminal.h"

/* File operation structs */
fops_t terminal_funcs =
{
	.read = terminal_read,
	.write = terminal_write,
	.open = terminal_open,
	.close = terminal_close
};

fops_t rtc_funcs =
{
	.read = rtc_read,
	.write = rtc_write,
	.open = rtc_open,
	.close = rtc_close
};

fops_t fsys_funcs =
{
	.read = file_read,
	.write = file_write,
	.open = file_open,
	.close = file_close
};

fops_t dir_funcs =
{
	.read = dir_read,
	.write = dir_write,
	.open = dir_open,
	.close = dir_close
};

boot_block_t* boot_blk;
int pid = 1;
static int32_t procs[MAX_DEVICES + 1] = {0};

/*
* add_process()
* DESCRIPTION: Adds a new process and returns its id if there's room
* INPUTS: None
* OUTPUT: returns process id on success, -1 on failure
*/
int32_t add_process(){
    uint32_t i;
    for(i = 1; i < MAX_DEVICES + 1; i++){
        if(procs[i] == 0){
            procs[i] = 1;
            return i;
        }
    }
    return -1;
}

/*
* delete_process(int32_t pid)
* DESCRIPTION: Removes a specified process
* INPUTS: pid
* OUTPUT: returns 0 on success, -1 on failure
*/
int32_t delete_process(int32_t p_id){
    if(p_id < 0 || p_id >= MAX_DEVICES){
        return -1;
    }
    procs[p_id] = 0;
    return 0;
}


/*
* int32_t read(int32_t fd, const uint8_t * buf, int32_t nbytes);
* DESCRIPTION: calls the read function for given file
* INPUTS: fd - file descriptor
*         buffer
*         nbytes - number of bytes
* OUTPUT: Return -1 on fail
*         Return corresponding function
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    /* Get current process control block from esp */
    // pcb_t* curr = get_PCB();

    /* Check for valid fd */
    /* fd = 1 is write only */
    if(buf == NULL || fd < 0 || fd > MAX_FD || fd == 1)
    {
        return -1;
    }

    /* check if fd has been opened */
    if(curr->file_array[fd].flags == 0)
    {
        return -1;
    }

    /* Return a call to the specified function */
    return (curr)->file_array[fd].fops->read(fd, buf, nbytes);
}

/*
* int32_t write(int32_t fd, const uint8_t * buf, int32_t nbytes);
* DESCRIPTION: calls the write function for rtc, file, or terminal
* INPUTS: fd - file descriptor
*         buffer
*         nbytes - number of bytes
* OUTPUT: Return -1 on fail
*         Return corresponding function
*/
int32_t write(int32_t fd, const void * buf, int32_t nbytes)
{
		/* Get current process control block from esp */
    //pcb_t* curr = get_PCB();

    /* Check for valid fd */
    /* fd = 0 is read only */
    if(buf == NULL || fd < 0 || fd > MAX_FD || fd == 0)
    {
        return -1;
    }

    /* check if fd has been opened */
    if(curr->file_array[fd].flags == 0)
    {
        return -1;
    }

    /* Return a call to the specified function */
    return (curr)->file_array[fd].fops->write(fd, buf, nbytes);
}
