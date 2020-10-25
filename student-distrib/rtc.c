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
#define RATE_MASK           0xF0 // Non-rate bits of register A

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

  /* Send end of interrupt signal */
  send_eoi(RTC_IRQ);
}

/* rtc_read
 *
 * DESCTIPTION: wait for interrupt
 * 
 * INPUT/OUTPUT: always returns 0
 * SIDE EFFECTS: makes thread wait
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    IR_flag = 0;
    while(!IR_flag); // wait for interrupt
    return 0;
}

/* rtc_write
 *
 * DESCTIPTION:
 * 
 * INPUT/OUTPUT:
 * SIDE EFFECTS:
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    int32_t frequency;
    uint8_t rate;

    /* should only accept a 4-byte integer as input */
    if (nbytes != 4 || buf == NULL)
        return -1;

    frequency = *((int32_t*)buf);

    /* get RTC rate from frequency */
    /* can only be power of 2 and no greater than 1024 */
    switch(frequency) {
        case 0:
            rate = 0x0;
        case 2:
            rate = 0xF;
        case 4:
            rate = 0xE;
        case 8:
            rate = 0xD;
        case 16:
            rate = 0xC;
        case 32:
            rate = 0xB;
        case 64:
            rate = 0xA;
        case 128:
            rate = 0x9;
        case 256:
            rate = 0x8;
        case 512:
            rate = 0x7;
        case 1024:
            rate = 0x6;
        default:
            return -1;
    }

    cli();
    outb((NMI | REG_A), RTC_PORT);
    outb((inb(CMOS_PORT) & RATE_MASK) | rate, CMOS_PORT);
    sti();
}

/* rtc_open
 *
 * DESCRIPTION: starts the irq, will set the default rate
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Starts this clock interrupt
 */
int32_t rtc_open(const uint8_t* filename) {

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

    return 0;
}

/* rtc_close
 *
 * DESCRIPTION: Masks interrupts so rtc doesn't work
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: disables irqs for rtc
 */
int32_t rtc_close(int32_t fd) {
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
