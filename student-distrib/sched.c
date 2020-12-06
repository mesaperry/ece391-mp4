#include "sched.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"

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

// Return the video address for the terminal associated with a given process
int32_t get_term_vid_addr(uint32_t proc)
{
  if(term_procs[proc] == 0)
  {
    return VIDEO_TERM_1;
  }
  else if(term_procs[proc] == 1)
  {
    return VIDEO_TERM_2;
  }
  else if(term_procs[proc] == 2)
  {
    return VIDEO_TERM_3;
  }

  return -1;
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
    /* Map virtual memory to appriate physical location */

    if (cur_p_id < 0) {
        execute(dechar("shell"));
    }

    // if next process is in open terminal, map virtual video memory to
    //   physical video memory
    if (proc_disp(next_p_id)) {
        map_v_p(VIDEO, VIDEO, 0, 1, 1);
    }

    // if next process is not in open terminal, map virtual video memory
    //   to task's non-display memory
    else {
        map_v_p(VIDEO, get_term_vid_addr(term_procs[next_p_id]), 0, 1, 1);
    }

    /* TASK SWITCH (do something similar to HALT) */
    /* Initialize variables */
	uint32_t esp;
	uint32_t ebp;
  uint32_t cur_pcb_ptr = find_PCB(cur_p_id);
	uint32_t next_pcb_ptr = find_PCB(next_p_id);

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
