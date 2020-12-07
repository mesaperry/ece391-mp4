#include "sched.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "syscalls.h"
#include "utils/char_util.h"
#include "lib.h"

// Find the current process id which the OS is currently giving CPU time to
int32_t cur_proc() {
    return running_procs[running_terminal];
}

// Find the next process id which the OS will give CPU time to next
int32_t next_proc() {
    return running_procs[(running_terminal + 1) % 3];
}

// Return whether a process is currently displayed. False: 0, True: 1
int32_t proc_disp(uint32_t proc) {
    if (term_procs[proc] == current_terminal) {
        return 1;
    } else {
        return 0;
    }
}

/* cycle_task
   DESCRIPTION: function to call for the OS to move to another task,
                  to be called periodically by PIT
   INPUTS: none
   OUTPUTS: none
   RETURNS: -1 on failure, 0 on success
   SIDE EFFECTS: switches tasks running in CPU
*/

uint32_t cycle_task() {
    /* Get next process id which we will switch to */
    uint32_t cur_p_id = cur_proc();
    uint32_t next_p_id = next_proc();
    uint32_t status = 1;
    /* Map virtual memory to appriate physical location */

    // TA_Q: If there is no shell yet, then start it here?
    if (cur_p_id < 0) {
        execute(dechar("shell"));
    }
/// yes, but need to save current esp and ebp, do not need to save eip, never return from line 46, need full algo for base shells

    // TA_Q: Can we map video like this:
    //   Make video_mem the normal VIDEO location if process is displayed
    //   Make video_mem the terminal's fake location if process is not displayed

//// mapping: yes exactly what you want to do

    // if next process is in open terminal, map virtual video memory to
    //   physical video memory
    if (proc_disp(next_p_id)) {
        video_mem = (char*) VIDEO;
    }

    // if next process is not in open terminal, map virtual video memory
    //   to task's non-display memory
    else {
        video_mem = (char*) get_term_vid_addr(term_procs[next_p_id]);
    }

    /* TASK SWITCH (do something similar to HALT) */

    // TA_Q: Which registers are required in task switch?
///// esp, ebp for the respective processes cur -> save esp and ebp NOT tss.esp0, next -> restore esp and ebp
        //// update tss.esp0 with next process esp0, need to save esp0 in execute code -> WE DO
    
    // TA_Q: Do we need to save the esp and ebp before we enter the function?

///// yes

    // TA_Q: Can this model work: Do a "halt-like" thing upon entering this
    //         function and then do an "execute-like" thing upon leaving?
    
///// not nessasarily, pit is faster than light, saving current state and restoring next, dont wanna execute new prog, pause and play

    /* Initialize variables */
	uint32_t esp;
	uint32_t ebp;
    pcb_t* cur_pcb_ptr = find_PCB(cur_p_id);
	pcb_t* next_pcb_ptr = find_PCB(next_p_id);

	/* NOT closing all the files in the pcb (like we do in halt)*/

    // TODO: DO SOMETHING WITH VIDMAP?
    //  if it was previously called on the incoming task, do we have to remap it?
    //  And if it was called on the outgoing task, do we have to unmap it?

    // The answer to this ^^ was no right? Because it should only be called for the main task that has the screen spaces
    // Although makes me wonder about if we run fish from multiple terminals


    /* === CONTEXT SWITCH === */
	/* Update stack pointers and base pointers */
    // TODO: PROBLEM:
    //  currently a task's esp and ebp is stored on its child's PCB.
    //  To make scheduling work, it needs to be stored on its own PCB
    //  so that it can restore itself with a round-robin approach rather
    //  than a parent-child approach

    // Switch current tasks esp/ebp to the one that's currently running on the next terminal
	esp = next_pcb_ptr->esp;
	ebp = next_pcb_ptr->ebp;

  /* Update TSS */
  tss.esp0 = (KERNEL_MEMORY_ADDR + MB_4) - (next_p_id) * PCB_SIZE - 4;

  /* Jump to execute return */
  /* exec_ret jumps to assembly in execute */
  flush_tlb();
//   asm volatile("           		  	\n\
//     movb    %0, %%bl				\n\
//     movl    %1, %%esp               \n\
//     movl    %2, %%ebp               \n\
//     jmp		exec_ret				\n\
//     "
//     :
//     : "r"(status), "r"(esp), "r"(ebp)
//     : "cc", "memory"
//   );

	// asm volatile("           		  	\n\
    // 	movb    %0, %%bl				\n\
	// 	movl    %1, %%esp               \n\
	// 	movl    %2, %%ebp               \n\
	// 	jmp		exec_ret				\n\
	// 	"
	// 	:
	// 	: "r"(status), "r"(esp), "r"(ebp)
	// 	: "cc", "memory"
	// );
	// return 0;
    return -1;
}
