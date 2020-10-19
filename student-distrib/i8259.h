/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT_ADD    0x20
#define MASTER_8259_PORT_DAT    0x21
#define SLAVE_8259_PORT_ADD     0xA0
#define SLAVE_8259_PORT_DAT     0xA1

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word
 *
 * https://courses.grainger.illinois.edu/ece391/fa2020/secure/references/8259A_PIC_Datasheet.pdf
 * Page 9 - 12
 */
#define ICW1                0x11 /* Starts the intialization sequence */

#define ICW2_MASTER         0x20 /* Handles page starting address of */
#define ICW2_SLAVE          0x28 /* certain routines */

#define ICW3_MASTER         0x04 /* Read when there's more than on 8529 */
#define ICW3_SLAVE          0x02 /* And is cascading */

#define ICW4                0x01 /* Deals with different modes */

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
