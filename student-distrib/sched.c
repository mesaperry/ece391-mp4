#include "sched.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "syscalls.h"
#include "utils/char_util.h"
#include "lib.h"

// Return whether a process is currently displayed. False: 0, True: 1
int32_t proc_disp(uint32_t proc) {
    if (term_procs[proc] == display_terminal) {
        return 1;
    } else {
        return 0;
    }
}

/* Scheduling initalize */
void sched_init() {
    display_terminal = 0;
    running_terminal = 0;

    /* Allocate multiple-terminal info */
    int x;
    for(x = 0; x < MAX_DEVICES; x++) {
        term_procs[x] = -1;
    }
    for (x = 0; x < MAX_TERMINAL_NUM; x++) {
        running_procs[x] = -1;
        map_v_p(get_term_vid_addr(x), get_term_vid_addr(x), 0, 1, 1);
        memcpy((uint32_t) get_term_vid_addr(x), (uint32_t) VIDEO, (uint32_t) PAGE_SIZE_KB);
    }
}

/*  cycle_task
*   DESCRIPTION: function to call for the OS to move to another task,
*                  to be called periodically by PIT
*   INPUTS: none
*   OUTPUTS: none
*   RETURNS: none
*   SIDE EFFECTS: switches tasks running in CPU
*/
void cycle_task() {
    switch_running_terminal((running_terminal + 1) % 3);
}

/*  switch_running_terminal
*   DESCRIPTION: function to call for the terminal to move to another task, on another terminal
*   INPUTS: The terminal to switch to
*   OUTPUTS: None
*   RETURNS: None
*   SIDE EFFECTS: switches tasks running in CPU
*/
void switch_running_terminal(uint32_t next_terminal) {
    /* Get next process id which we will switch to */
    int32_t cur_p_id = running_procs[running_terminal];
    int32_t next_p_id = running_procs[next_terminal];

    if (next_p_id < 0) {
        set_video_mem((char*) VIDEO);
        execute(dechar("shell"));
    }

    running_terminal = next_terminal;

    // SAVE ESP/EBP
    pcb_t* cur_pcb_ptr = find_PCB(cur_p_id);
    asm volatile ("                               \n\
        movl %%esp, %0                            \n\
        movl %%ebp, %1                            \n\
        "
        : "=r"(cur_pcb_ptr->esp), "=r"(cur_pcb_ptr->ebp)
        : /* no inputs */
        : "cc"
    );

    /* Map memory to appriate physical location */
    // if next process is in open terminal, map virtual video memory to
    //   physical video memory
    if (term_procs[next_p_id] == display_terminal) {
        set_video_mem((char*) VIDEO);
    }

    // if next process is not in open terminal, map virtual video memory
    //   to task's non-display memory
    else {
        set_video_mem((char*) get_term_vid_addr(term_procs[next_p_id]));
    }

    /* CONTEXT SWITCH (do something similar to HALT) */

    // TODO: DO SOMETHING WITH VIDMAP?
    //  if it was previously called on the incoming task, do we have to remap it?
    //  And if it was called on the outgoing task, do we have to unmap it?

    // The answer to this ^^ was no right? Because it should only be called for the main task that has the screen spaces
    // Although makes me wonder about if we run fish from multiple terminals


    /* === CONTEXT SWITCH === */
	/* Update stack pointers and base pointers */

    /* Restore next process' esp/ebp */
    pcb_t* next_pcb_ptr = find_PCB(next_p_id);
    asm volatile("           		  	\n\
        movl    %0, %%esp               \n\
        movl    %1, %%ebp               \n\
        "
        :
        : "r"(next_pcb_ptr->esp), "r"(next_pcb_ptr->ebp)
        : "cc", "memory"
    );

    /* Update TSS */
    tss.esp0 = next_pcb_ptr->esp;

    flush_tlb();
}
