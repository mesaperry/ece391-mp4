#ifndef _SCHED_H
#define _SCHED_H
#include "types.h"

#define MAX_TERMINAL_NUM 3
#define MAX_DEVICES 6

uint8_t display_terminal;  // The currently displayed terminal
uint8_t running_terminal;  // The currently running terminal

int32_t term_procs[MAX_DEVICES];  // Says which process is running on which terminal
int32_t running_procs[MAX_TERMINAL_NUM];  // The foremost processes in each terminal

/* Move to the next scheduled task */
extern void cycle_task();
/* Move to the task in the given terminal */
extern void switch_running_terminal();

#endif /* _SCHED_H */
