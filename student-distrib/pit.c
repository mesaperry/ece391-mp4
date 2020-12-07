/* pit.c - Init for PIT */

#include "pit.h"
#include "x86_desc.h"
#include "i8259.h"
#include "sched.h"
#include "utils/char_util.h"
#include "syscalls.h"

#define INT_INTERVAL                 15 // timer interrupt interval in ms
#define PIT_IRQ                     0x00

#define MAX_FREQ                    1193182
#define MS_IN_SEC                   1000
#define PIT_MODE3                   0x36
#define PIT_CH0                     0x40
#define PIT_CMD                     0x43
#define BYTE_MASK                   0xFF
#define BYTE_SIZE                   8


void pit_handler() {
    if (cycle_task()) {
        send_eoi(PIT_IRQ);
        execute(dechar("shell"));
    }
    send_eoi(PIT_IRQ);
}

/*  pit_init
 *  DESCRIPTION: Initalizes PIT and sets channel 0 starting frequency
 *  INPUT: None
 *  OUTPUT: None
 *  RETURNS: None
 *  SIDE EFFECTS: enables the PIT on the PIC and give a starting frequency
 */
void pit_init() {
    uint16_t divisor = MAX_FREQ * INT_INTERVAL / MS_IN_SEC;

    outb(PIT_MODE3, PIT_CMD);
    outb((uint8_t) (divisor & BYTE_MASK), PIT_CH0);
    outb((uint8_t) ((divisor >> BYTE_SIZE) & BYTE_MASK), PIT_CH0);

    enable_irq(PIT_IRQ);
}
