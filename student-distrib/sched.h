#ifndef _SCHED_H
#define _SCHED_H
#include "types.h"

#define VIDEO_TERM_1 0xC0000
#define VIDEO_TERM_2 0xC1000
#define VIDEO_TERM_3 0xC2000

/* Move to the next scheduled task */
extern uint32_t cycle_task();

#endif /* _SCHED_H */
