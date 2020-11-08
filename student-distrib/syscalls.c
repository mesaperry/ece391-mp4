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
	// .read = dir_read, NOT WRITTEN YET
	.write = dir_write,
	.open = dir_open,
	.close = dir_close
};

int32_t halt(uint8_t status)
{
	// TODO
	return -1;
}

int32_t execute (const uint8_t* command)
{
	// TODO
	return -1;
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
	// TODO implement
    /* Get current process control block from esp */
    // pcb_t* curr = get_PCB();

    // /* Check for valid fd */
    // /* fd = 1 is write only */
    // if(buf == NULL || fd < 0 || fd > MAX_FD || fd == 1)
    // {
    //     return -1;
    // }
	//
    // /* check if fd has been opened */
    // if(curr->file_array[fd].flags == 0)
    // {
    //     return -1;
    // }
	//
    // /* Return a call to the specified function */
    // return (curr)->file_array[fd].fops->read(fd, buf, nbytes);
	return -1;
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
	// TODO implement
    // pcb_t* curr = get_PCB();
	//
    // /* Check for valid fd */
    // /* fd = 0 is read only */
    // if(buf == NULL || fd < 0 || fd > MAX_FD || fd == 0)
    // {
    //     return -1;
    // }
	//
    // /* check if fd has been opened */
    // if(curr->file_array[fd].flags == 0)
    // {
    //     return -1;
    // }
	//
    // /* Return a call to the specified function */
    // return (curr)->file_array[fd].fops->write(fd, buf, nbytes);
	return -1;
}

int32_t open (const uint8_t* filename)
{
	// TODO
	return -1;
}

int32_t close(int32_t fd)
{
	// TODO
	return -1;
}
