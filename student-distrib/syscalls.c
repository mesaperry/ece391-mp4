/* Created by Team 11 */
#include "syscalls.h"

#include "paging.h"
#include "lib.h"
#include "x86_desc.h"
#include "terminal.h"
//#include "syscalls.S"

#include "utils/arg_util.h"
#include "utils/file_util.h"
#include "utils/char_util.h"
#include "filesys.h"

static uint32_t process_count = 0;

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
uint8_t vidmap_called = 0;

/*
* add_process()
* DESCRIPTION: Adds a new process and returns its id if there's room
* INPUTS: None
* OUTPUT: returns process id on success, -1 on failure
*/
int32_t add_process(){
    uint32_t i;		/* Loop through available indices */
    for(i = 0; i < MAX_DEVICES; i++){
			/* If processindex is available, return the index */
        if(procs[i] == 0){
            procs[i] = 1;
			set_term_process(i);
			process_count++;
			running_procs[current_terminal] = i;
			term_procs[i] = current_terminal;
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
	remove_term_process(pid);
	process_count--;
	running_procs[current_terminal] = find_PCB(pid)->par_p_id;
	term_procs[pid] = -1;
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
	pcb_t* pcb_child_ptr;
	pcb_t* pcb_parent_ptr = NULL;
	uint32_t esp;
	uint32_t ebp;
	uint32_t i;
	uint32_t physical_addr;

	/* Restore parent data */
	pcb_child_ptr = get_current_PCB();

	/* Setting parent ptr, if it exists */
	if (pcb_child_ptr->par_p_id >= 0) {
		pcb_parent_ptr = find_PCB(pcb_child_ptr->par_p_id);

		/* If there are still active PCBs, map virtual address to parent pointers p_id */
		physical_addr = USER_PROCESS_START_PHYSICAL + pcb_parent_ptr->p_id * USER_PROCESS_SIZE;
		map_v_p(USER_PROCESS_START_VIRTUAL, physical_addr, 1, 1, 1);

		/* Write parent's process info into TSS */
		tss.esp0 = (KERNEL_MEMORY_ADDR + MB_4) - (pcb_parent_ptr->p_id) * PCB_SIZE - 4;

		/* Set stack pointer to previous PCB's location */
		/* Kernel_mode_stack address here */
		esp = pcb_parent_ptr->esp;
		ebp = pcb_parent_ptr->ebp;
		delete_process(pcb_child_ptr->p_id);
	} else {
		delete_process(pcb_child_ptr->p_id);
		asm volatile("           		  	\n\
			movb    %0, %%bl				\n\
			jmp		exec_ret				\n\
			"
			:
			: "r"(status)
			: "cc", "memory"
		);
	}

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

	/* Unmap if vidmap was called */
	if(vidmap_called)
	{
		/* Unmap 4kb page from user video memory */
		map_v_p(USER_VIDMAP, 0, 0, 1, 1);
	}

	putc('\n');

	// TODO: TSS?

	/* Jump to execute return */
	/* exec_ret jumps to assembly in execute */
	flush_tlb();
	asm volatile("           		  	\n\
    	movb    %0, %%bl				\n\
		movl    %1, %%esp               \n\
		movl    %2, %%ebp               \n\
		jmp		exec_ret				\n\
		"
		:
		: "r"(status), "r"(esp), "r"(ebp)
		: "cc", "memory"
	);
	return 0;
}

int32_t execute (const uint8_t* all_arguments)
{
	pcb_t* pcb;
	uint8_t all_arguments_copy[MAX_BUFF_LENGTH];
	int32_t command_length, process_id, parent_process_id, i, output;
	uint32_t virtual_stack_addr, physical_addr;
	dentry_t dentry;
	fd_t stdin;
	fd_t stdout;

	copy_string(all_arguments, all_arguments_copy);

	/* Get first word (command) */
	command_length = get_argument_length(all_arguments_copy, 0);
	if (command_length < 0) return 0;  // Error getting first word

	uint8_t executable[command_length + 1];
	executable[command_length] = '\0';  // Make it a string by adding EOS
	if (get_argument(all_arguments_copy, 0, executable) < 0) return 0;  // Error copying argument

	/* Check that command is an executable */
	if (!is_executable(executable)) return -1;

	/* Paging: load user level program loaded in page starting at 128MB */
	/*           and physical memory starts at 8MB + (pid * 4MB)        */

	/* Update process ids */
	parent_process_id = running_procs[current_terminal];
	process_id = add_process();
	if (process_id < 0) return -1;  // add process failed

	/* Create next PCB */
	pcb = (pcb_t*)((KERNEL_MEMORY_ADDR + MB_4) - (process_id + 1) * PCB_SIZE);

	/* Save extra parameters to global variable, stripped of leading spaces */
	get_next_arguments(all_arguments_copy, pcb->arg_buffer);

	/* Need to double check the values i the below formulas */
	physical_addr = USER_PROCESS_START_PHYSICAL + process_id * USER_PROCESS_SIZE;
	virtual_stack_addr = (uint32_t)USER_PROCESS_STACK;

	/* Map user location, supervisor protection so we can copy program */
	map_v_p(USER_PROCESS_START_VIRTUAL, physical_addr, 1, 1, 1);

	/* User Level program loading:                               */
	/*   Copy file contents to correct location                  */
	/*   Find the first instruction's address                    */
	read_dentry_by_name(executable, &dentry);
	read_file_bytes_by_name(executable, (uint8_t*)(USER_PROCESS_START_VIRTUAL + USER_PROCESS_IMAGE_OFFSET), file_size(executable));

	/* Create eip_buf to put into EIP location in asm */
	uint32_t* eip_ptr = (uint32_t*)(USER_PROCESS_START_VIRTUAL + USER_PROCESS_IMAGE_OFFSET + ELF_OFFSET);
	uint8_t eip_buf[4];
	for (i = 0; i < 4; i++)
	{
  		eip_buf[i] = *(uint8_t*)(USER_PROCESS_START_VIRTUAL + USER_PROCESS_IMAGE_OFFSET + ELF_OFFSET + 3 - i);
	}

	/* Set fd = 0 and fd = 1 in file_array */
	stdin.fops = &terminal_funcs;
	stdin.inode = NULL;
	stdin.pos = 0;
	stdin.flags = 1;
	pcb->file_array[0] = stdin;

	stdout.fops = &terminal_funcs;
	stdout.inode = NULL;
	stdout.pos = 0;
	stdout.flags = 1;
	pcb->file_array[1] = stdout;


	/* Initialize remaining file array */
	for(i = 2; i < FILE_ARRAY_LEN; i++)
	{
		pcb->file_array[i].fops = NULL;
		pcb->file_array[i].inode = NULL;
		pcb->file_array[i].pos = 0;
		pcb->file_array[i].flags = 0;
	}

	/* Update PCB values */
	pcb->p_id = process_id;
	pcb->par_p_id = parent_process_id;

	tss.esp0 = (KERNEL_MEMORY_ADDR + MB_4) - (process_id) * PCB_SIZE - 4;
	tss.ss0 = KERNEL_DS;
	if (parent_process_id >= 0) {
		asm volatile ("                               \n\
			movl %%esp, %0                            \n\
			movl %%ebp, %1                            \n\
			"
			: "=r"(find_PCB(parent_process_id)->esp), "=r"(find_PCB(parent_process_id)->ebp)
			: /* no inputs */
			: "cc"
		);
	}


	asm volatile("          				\n\
		mov    $0x2B, %%ax                  \n\
		mov    %%ax, %%ds                   \n\
		pushl	$0x2B						\n\
		pushl	%3							\n\
		pushfl								\n\
		popl    %%eax 						\n\
		orl     $0x200, %%eax				\n\
		pushl   %%eax 						\n\
		pushl	$0x23						\n\
		pushl	%2							\n\
		iret								\n\
		"
		:
		: "r"(USER_DS), "r"(USER_CS), "r"(*(uint32_t*)(eip_ptr)), "r"(virtual_stack_addr)//, "r"(), "r"()
		: "cc", "memory"
	);

	asm volatile ("	\n\
		exec_ret:							\n\
		xorl %%eax, %%eax								\n\
		mov %%bl, %0					\n\
	"
	: "=rm"(output)
	:
	: "cc", "memory"
	);
	if (running_procs[current_terminal] < 0) {
		printf("Haha! Nice try. Restarting shell...\n");
		execute(dechar("shell"));
	}
	return output;
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
    if(fd < 0 || fd > MAX_FD || fd == 0)
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
				if(copy_string(filename, fd_ptr->file_name) == -1)
				{
					for(index = 0; index < FNAME_MAX_LEN; index++)
					{
						fd_ptr->file_name[index] = (uint8_t)'\0';
					}
				}
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

/* getargs
 * DESCRIPTION: Put the command arguments from the user program into a buffer
 * INPUTS: buf    -- a buffer in which to put the bytes
 *         nbytes -- number of bytes to copy
 * OUTPUTS: arguments into buf
 * RETURNS: -1 if arguments can't fit, 0 if success
 * SIDE EFFECTS: none
 */
int32_t getargs (uint8_t* buf, uint32_t nbytes)
{
	pcb_t* pcb = get_current_PCB();
	/* pcb->arg_buffer must have EOS */
	if ((string_length(pcb->arg_buffer) + 1) > nbytes) return -1;
	copy_string(pcb->arg_buffer, buf);
	/* another option... */
	// copy_buf(pcb->arg_buffer, buf, nbytes);
	printf("getargs buf: %s\n", buf);
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


/* Checkpoint 4 Syscalls */
/*
* int32_t vidmap (uint8_t** screen_start);
* DESCRIPTION:  Maps the text-mode video memory into user space at a pre-determined virtual address
* INPUTS: screen start - Pointer to start os user video memory
* OUTPUT: Return -1 on fail,return virtual address on success
* SIDE EFFECTS: Alters the physical memory buffer
*/
int32_t vidmap (uint8_t** screen_start)
{
	 /* Check if address is valid */
	if(screen_start == NULL)
	{
		return -1;
	}

	/* Check if outside of program boundaries */
	if((uint32_t)screen_start < (USER_PROCESS_START_VIRTUAL + USER_PROCESS_IMAGE_OFFSET)
	 	|| (uint32_t)screen_start >= (USER_PROCESS_START_VIRTUAL + MB_4))
	{
		return -1;
	}

	vidmap_called = 1;

	/* Map 4kb page from user memory, point it to physical video memory */
	map_v_p(USER_VIDMAP, VIDEO, 0, 1, 1);

	// Set screen start to 64 MB
	*screen_start = (uint8_t*)(USER_VIDMAP);
	return USER_VIDMAP;
}

/*
* int32_t set_handler (int32_t signum, void* handler_address);
* DESCRIPTION: Sets a given signal to be handled by given handler
* INPUTS: signum 		  -	The signal to be handeled
		  handler_address - The location of the handler for given signal
* OUTPUT: Return -1 on fail
* SIDE EFFECTS:
*/
int32_t set_handler (int32_t signum, void* handler_address)
{
	return -1;
}

/*
* int32_t sigreturn (void);
* DESCRIPTION:
* INPUTS:
* OUTPUT: Return -1 on fail
* SIDE EFFECTS:
*/
int32_t sigreturn (void)
{
	return -1;
}
