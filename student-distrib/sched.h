#ifndef _SCHED_H
#define _SCHED_H
#include "types.h"

#define MAX_TERMINAL_NUM 3
#define MAX_DEVICES 6

uint32_t display_terminal;  // The currently displayed terminal
uint32_t running_terminal;  // The currently running terminal

int32_t term_procs[MAX_DEVICES];  // Says which process is running on which terminal
int32_t running_procs[MAX_TERMINAL_NUM];  // The foremost processes in each terminal

/* Initialize scheduler */
extern void sched_init();
/* Move to the next scheduled task */
extern uint32_t cycle_task();
/* Move to the task in the given terminal */
extern uint32_t switch_running_terminal();

#endif /* _SCHED_H */
