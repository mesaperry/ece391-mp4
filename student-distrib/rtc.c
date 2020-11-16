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

#define MAX_RATE            0x06
#define MAX_FREQ            1024

static const int32_t DEF_FREQ = 2;

volatile int32_t IR_flag;
volatile int32_t clk;
volatile int32_t virtual_freq;


extern void rtc_intr();

/* rtc_init
 *
 * DESCRIPTION: Initializes RTC in IDT
 *
 * INPUT/OUTPUT: None
 * SIDE EFFECTS: Sets up RTC IDT
 */
void rtc_init() {
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_intr);
}

/* rtc_handler
 *
 * DESCRIPTION: Parses calls to RTC
 *
 * INPUT/OUTPUT: None
 * SIDE EFFECTS: Sends end of interrupt signal
 */
void rtc_handler() {
    /* Increment clk counter */
    clk = (clk + 1) % MAX_FREQ;

    /* Signal IR_flag */
    if (virtual_freq != 0 && clk % virtual_freq == 0) {
        IR_flag = 1;
    }

    /* Select Register C and throw away contents */
    outb(REG_C, RTC_PORT);
    inb(CMOS_PORT);

    /* Send end of interrupt signal */
    send_eoi(RTC_IRQ);
}

/* rtc_read
 *
 * DESCTIPTION: Waits for interrupt
 *
 * INPUT/OUTPUT: Always returns 0
 * SIDE EFFECTS: Makes thread wait
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    IR_flag = 0;
    sti();
    while(!IR_flag); // wait for interrupt
    cli();
    return 0;
}

/* rtc_write
 *
 * DESCTIPTION: Sets RTC clock interrupt rate in Hz
 *
 * INPUT/OUTPUT: Inputs rate, outputs 0 on success or -1 on failure
 * SIDE EFFECTS: Adjusts clock rate
 */
int32_t rtc_write( int32_t fd,
                   const void* buf,
                   int32_t nbytes ) {
    int32_t frequency;                      // input frequency

    /* verify input is a 4-byte int */
    if (nbytes != 4 || buf == NULL) {
        return -1;
    }

    frequency = *((int32_t*)buf);

    /* verify frequency is no greater than 1024 Hz */
    if (frequency > 1024) {
        return -1;
    }

    /* verify frequency is not 1 Hz */
    if (frequency == 1) {
        return -1;
    }

    /* verify input is 0 or a power of 2 */
    if ((frequency & (frequency - 1)) != 0) {
        return -1;
    }

    virtual_freq = frequency;

    return 0;
}

/* rtc_open
 *
 * DESCRIPTION: Enables RTC IRQ, initializes to default rate
 *
 * INPUT/OUTPUT: Always returns 0
 * SIDE EFFECTS: Starts RTC interrupts
 */
int32_t rtc_open(const uint8_t* filename) {

    /* Clear IF */
    cli();

    /* Turn on IRQ 8 */
    /* Select Reg B and disable NMI */
    outb(REG_B | NMI, RTC_PORT);

    /* Write prev value, or'd with 0x40 */
    outb(PI | inb(CMOS_PORT), CMOS_PORT);

    /* Set physical frequency to max */
    cli();
    outb((NMI | REG_A), RTC_PORT);
    outb((inb(CMOS_PORT) & RATE_MASK) | MAX_RATE, CMOS_PORT);

    /* Set virtual frequency to default */
    rtc_write(0, &DEF_FREQ, sizeof(DEF_FREQ));

    /* Reset clk counter */
    clk = 0;

    /* Enable interrupt */
    enable_irq(RTC_IRQ);

    return 0;
}

/* rtc_close
 *
 * DESCRIPTION: Disables RTC IRQ
 *
 * INPUT/OUTPUT: Always returns 0
 * SIDE EFFECTS: Disables RTC interrupts
 */
int32_t rtc_close(int32_t fd) {
    /* Disable IRQs */
    disable_irq(RTC_IRQ);

    /* Clear IF */
    cli();

    /* Turn on IRQ 8 */
    /* Select Reg B and disable NMI */
    outb((NMI | REG_B), RTC_PORT);
    outb((inb(CMOS_PORT) & ~PI), CMOS_PORT);

    return 0;
}

/* rtc_get_rate
 *
 * DESCRIPTION: Gets the current RTC rate
 *
 * INPUT/OUTPUT: Returns RTC rate
 * SIDE EFFECTS: None
 */
uint8_t rtc_get_rate() {
    uint8_t rate;

    cli();
    outb((NMI | REG_A), RTC_PORT);
    rate = inb(CMOS_PORT) & ~RATE_MASK;

    return rate;
}

/* rtc_is_on
 *
 * DESCRIPTION: Checks if IRQ 8 is on
 *
 * INPUT/OUTPUT: Returns 1 if on, 0 if off
 * SIDE EFFECTS: None
 */
int rtc_is_on() {
    int is_on;

    cli();
    outb(REG_B | NMI, RTC_PORT);
    is_on = (PI & inb(CMOS_PORT)) != 0;

    return is_on;
}
