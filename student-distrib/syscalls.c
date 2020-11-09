/* Created by Team 11 */
#include "syscalls.h"
#include "paging.h"
#include "lib.h"
#include "x86_desc.h"
#include "terminal.h"

#include "utils/arg_util.h"
#include "utils/file_util.h"
#include "filesys.h"

uint32_t process_count = 0;

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

static int32_t procs[MAX_DEVICES] = {0};

uint8_t command_arguments[MAX_BUFF_LENGTH];

/*
* add_process()
* DESCRIPTION: Adds a new process and returns its id if there's room
* INPUTS: None
* OUTPUT: returns process id on success, -1 on failure
*/
int32_t add_process(){
    uint32_t i;

		/* Loop through available indices */
    for(i = 0; i < MAX_DEVICES; i++){
			/* If processindex is available, return the index */
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
int32_t delete_process(int32_t pid){
	/* Validate the input pid */
    if(pid < 0 || pid >= MAX_DEVICES){
        return -1;
    }
		/* Free space for process pid */
    procs[pid] = 0;
    return 0;
}

/*
* int32_t halt(int32_t fd);
* DESCRIPTION: clean-up process
* INPUTS: status
*/
int32_t halt (uint8_t status)
{
	/* Initialize variables */
	pcb_t* pcb_parent_ptr = NULL;
	pcb_t* pcb_child_ptr;
	uint32_t esp;
	uint32_t ebp;
	uint32_t i;

	/* Restore parent data */
	pcb_child_ptr = get_current_PCB();

	/* */
	if(pcb_child_ptr->p_id > 0)
	{
		pcb_parent_ptr = find_PCB((pcb_child_ptr->p_id) - 1);
	}

	delete_process(pcb_child_ptr->p_id);
	process_count--;

	/* Close all the files in the pcb */
	for(i = 0; i < FILE_ARRAY_LEN; i++)
	{
		if (pcb_child_ptr->file_array[i].flags == 1)
		{
			close(i);
		}
		pcb_child_ptr->file_array[i].fops = NULL;
		pcb_child_ptr->file_array[i].flags = 0;
	}

	/* Re-map memory */

	/* Restore parent paging */
	if(pcb_child_ptr->p_id > 0)
	{
		/* Set stack pointer to previous PCB's location */
		esp = pcb_parent_ptr->esp;
	}
	else
	{
		/* Set stack pointer to top of kernel */
		esp = KERNEL_MEMORY_ADDR;
	}

	/* Write parent's process info into TSS */
	tss.esp0 = esp;

	/* Jump to execute return */
	/* halt_ret_label jumps to assembly in execute */
	/*asm volatile("              \n\
			movl    %1, %%esp       \n\
			movl    %2, %%ebp       \n\
			movb    %0, %%bl        \n\
			jmp     halt_ret_label  \n\
			"
			:
			: "r" (status), "r" (esp), "r" (ebp)
			: "cc", "memory"
	);*/
	return 0;
}

int32_t execute (const uint8_t* command)
{
	/* Get first word (command) */
	int32_t command_length = get_argument_length(command, 0);
	if (command_length < 0) return 0;  // Error getting first word

	uint8_t executable[command_length + 1];
	executable[command_length] = '\0';  // Make it a string by adding EOS
	if (get_argument(command, 0, executable) < 0) return 0;  // Error copying argument

	/* Check that command is an executable */
	if (!is_executable(executable)) return -1;

	/* Save extra parameters to global variable, stripped of leading spaces */
	get_next_arguments(command, command_arguments);

	/* Paging: load user level program loaded in page starting at 128MB */
	/*           and physical memory starts at 8MB + (pid * 4MB)        */

	int32_t process_id = add_process();
	if (process_id < 0) return -1;  // add process failed

	uint32_t virtual_addr = 0x800000 + process_id*0x40000;
	uint32_t physical_addr = 0x40000 + process_id*0x40000
	map_v_p(virtual_addr, physical_addr, 1)

	/* User Level program loading:                               */
	/*   Copy file contents to correct location                  */
	/*   Find the first instruction's address                    */
	dentry_t dentry;
	read_dentry_by_name(executable, &dentry);
	read_file_bytes_by_name(executable, (uint8_t*)virtual_addr, dentry.inode_index.length);

	/* Create PCB */
	pcb_t pcb;

	// asm volatile ("                 \n\
	// // TODO initialize these values:             \n\
	// 	// fd_t file_array[FILE_ARRAY_LEN];      \n\
	// 	// 	int8_t arg_buffer[MAX_BUFF_LENGTH];  \n\
	// 	// int32_t p_id; \n\
	// 	// uint32_t esp; \n\
	// 	// uint32_t ebp; \n\
	// 	// uint32_t esi; \n\
	// 	// uint32_t edi; \n\
	// 	// uint32_t eax; \n\
	// 	// uint32_t ebx; \n\
	// 	// uint32_t ecx; \n\
	// 	// uint32_t edx; \n\
    //                              \n\
	// /* Context Switch */         \n\
	// // TODO!!
	// 		"
	// 		:
	// 		:
	// 		:
	// );

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
    /* Get current process control block*/
    pcb_t* curr = get_current_PCB();

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
		/* Get current process control block*/
  	pcb_t* curr = get_current_PCB();

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

/*
* int32_t open(uint8_t* filename);
* DESCRIPTION: calls the open function for rtc, file, or terminal
* INPUTS: fd - file descriptor
*         buffer
*         nbytes - number of bytes
* OUTPUT: Return -1 on fail
*         Return corresponding function
*/
int32_t open(const uint8_t* filename)
{
		/* Initialize variables */
    int32_t fd = -1;
    dentry_t dentry;
    fd_t* fd_ptr = NULL;
		uint32_t index;

		/* Get current PCB */
		pcb_t* pcb = get_current_PCB();

    for(index = 2; index < FILE_ARRAY_LEN; index++)
		{
				/* Find an unused file in file_array */
        if(pcb->file_array[index].flags == 0 && fd < 2)
				{
            fd = index;
            break;
        }
    }

		/* Return -1, if fd never set and all files in use */
    if(fd == -1)
		{
        return fd;
    }

		// Get pointer to the current file_array
    fd_ptr = &(pcb->file_array[fd]);

		/* Get dentry based on filename */
    if(read_dentry_by_name(filename, &dentry) == -1)
		{
        return -1;
    }

		/* Handle different file types */
    if(dentry.file_type == 0) // RTC File
		{
        fd_ptr->inode = NULL;
        fd_ptr->pos = 0;
        fd_ptr->flags = 1;
        fd_ptr->fops = &rtc_funcs;
    }
    else if(dentry.file_type == 1) // Directory
		{
        fd_ptr->inode = NULL;
        fd_ptr->pos = 0;
        fd_ptr->flags = 1;
        fd_ptr->fops = &dir_funcs;
    }
    else if(dentry.file_type == 2) // Regular File
		{
        fd_ptr->inode = dentry.inode_index;
        fd_ptr->pos = 	0;
        fd_ptr->flags = 1;
        fd_ptr->fops = &fsys_funcs;
    }
    else
		{
        return -1;
    }

		/* Return fd that's been opened */
    return fd;
}

/*
* int32_t close(int32_t fd);
* DESCRIPTION: calls the close function for rtc, file, or terminal
* INPUTS: fd - file descriptor
*         buffer
*         nbytes - number of bytes
* OUTPUT: Return -1 on fail
*         Return corresponding function
*/
int32_t close(int32_t fd)
{
		/* Get PCB of current process block */
    pcb_t* curr = get_current_PCB();

    /* Check if descriptor is valid and file is in use */
    if(fd < MIN_FD || fd > MAX_FD || curr->file_array[fd].flags == 0)
		{
        return -1;
    }

    /* Return fail if close function is unsuccessful */
    if((curr)->file_array[fd].fops->close(fd) != 0)
		{
        return -1;
    }

		/* Re-set file with default values */
    curr->file_array[fd].fops = NULL;
    curr->file_array[fd].pos = 0;
    curr->file_array[fd].flags = 0;

		/* Return 0 on success */
    return 0;
}

/*
* find_PCB()
* DESCRIPTION: Finds the PCB of the given p_id
* INPUTS: p_id
* OUTPUT: Returns the PCB
*/
pcb_t* find_PCB(int pid) {
	 	/* Return PCB location in kernel stack, using p_id offset */
    return (pcb_t*)(MB_8 - (KB_8 * (pid + 1)));
}

/*
* get_current_PCB()
* DESCRIPTION: gets the current PCB
* INPUTS: None
* OUTPUT: Returns the current PCB
*/
pcb_t* get_current_PCB() {
    uint32_t esp;
    asm volatile (              \
        "movl %%esp, %0"        \
        : "=rm" (esp)           \
    );
    return (pcb_t*)(esp & ESP_MASK);
}
