/* rtc.c - real time clock implementation
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"

#define RTC_IRQ             0x08 // Port on Slave PIC
#define RTC_VEC             0x28 // IDT Vector

#define RTC_PORT            0x70
#define CMOS_PORT           0x71

#define NMI                 0x80 // Non-Mask Interrupt(NMI) Bit
#define PI                  0x40 // Periodic Interrupt(PI) Bit

#define REG_A               0x0A // Registers
#define REG_B               0x0B
#define REG_C               0x0C

volatile int32_t IR_flag;


extern void rtc_intr();

/* rtc_init
 *
 * DESCRIPTION: Initializes RTC in idt
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Sets up IDT
 */
void rtc_init() {
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_intr);
}

/* rtc_handler
 *
 * DESCRIPTION: Parses calls to RTC
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Sends end of interrupt signal
 */
void rtc_handler() {
  /* Raise interrupt flag */
  IR_flag = 1;

  /* Select Register C and throw away contents */
  outb(REG_C, RTC_PORT);
  inb(CMOS_PORT);

  /* Test for checkpoint 1 */
//   test_interrupts();

  /* Send end of interrupt signal */
  send_eoi(RTC_IRQ);
}

/* rtc_open
 *
 * DESCRIPTION: starts the irq, will set the default rate
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Starts this clock interrupt
 */
void rtc_open() {

    /* Open critical section */
    cli();

    /* Turn on IRQ 8 */
    /* Select Reg B and disable NMI */
    outb(REG_B | NMI, RTC_PORT);

    /* Write prev value, or'd with 0x40 */
    outb(PI | inb(CMOS_PORT), CMOS_PORT);

    /* Close critical section */
    sti();

    /* Enable interrupt */
    enable_irq(RTC_IRQ);
}

/* rtc_close
 *
 * DESCRIPTION: Masks interrupts so rtc doesn't work
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: disables irqs for rtc
 */
int32_t rtc_close() {
    /* Disable IRQs */
    disable_irq(RTC_IRQ);

    /* Open critical section */
    cli();

    /* Turn on IRQ 8 */
    /* Select Reg B and disable NMI */
    outb((NMI | REG_B), RTC_PORT);
    outb((inb(CMOS_PORT) & ~PI), CMOS_PORT);

    /* Close critical section */
    sti();

    return 0;
}
