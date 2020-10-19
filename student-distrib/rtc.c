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


extern void rtc_irq();

void rtc_init() {
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_irq);
}

void rtc_handler() {
  /* Raise interrupt flag */
  IR_flag = 1;

  /* Select Register C and throw away contents */
  outb(REG_C, RTC_PORT);
  inb(CMOS_PORT);

  /* Send end of interrupt signal */
  send_eoi(RTC_IRQ);
}

void rtc_enable() {

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

void rtc_disable() {
    disable_irq(RTC_IRQ);
}
