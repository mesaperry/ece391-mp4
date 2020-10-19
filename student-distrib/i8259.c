/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
  /* ICW 1 */
  /*
		Upon initialization, i8259 should use edge-triggered input signals.
		Cascade them, as there's more than one i8259
	*/
  outb(ICW1, MASTER_8259_PORT);
  outb(ICW1, SLAVE_8259_PORT);

  /* ICW 2 */
  /*
		Vector offset to map master (x20-x27) and slave (x28-x2F) interrupt vectors
	*/
  outb(ICW2_MASTER, MASTER_8259_PORT_DAT);
  outb(ICW2_SLAVE, SLAVE_8259_PORT_DAT);

  /* ICW 3 */
  /*
		Specify IR pin for master/slave orientation
	*/
  outb(ICW3_MASTER, MASTER_8259_PORT_DAT);
  outb(ICW3_SLAVE, SLAVE_8259_PORT_DAT);

  /* ICW4 */
	/*
		8086 protocol, normal EOI signaling, and other necessary info
	*/
	outb(ICW4, MASTER_8259_PORT_DAT);
	outb(ICW4, SLAVE_8259_PORT_DAT);

  /* Mask interrupts before enable */
	outb(0xFF, MASTER_8259_PORT_DAT);
	outb(0xFF, SLAVE_8259_PORT_DAT);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {

  uint16_t port = MASTER_8259_PORT_DAT;

	if((irq_num >= IRQ_AMNT) && (irq_num < IRQ_AMNT*2)) {
		/* Masking IRQ2 will cause the Slave PIC to stop raising IRQs. */
		outb(inb(port) & ~(1 << IRQ2), port);
		irq_num %= IRQ_AMNT; /* Change it within master range */
		port = SLAVE_8259_PORT_DAT;

	}

	outb(inb(port) & ~(1 << irq_num), port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  uint16_t port = MASTER_8259_PORT_DAT;

	if((irq_num >= 0) && (irq_num < IRQ_AMNT)) {
		/* Masking IRQ2 will cause the Slave PIC to stop raising IRQs. */
		outb(inb(port) | (1 << IRQ2), port);
		irq_num %= IRQ_AMNT; //Change it withing master range
		port = SLAVE_8259_PORT_DAT;
	}

	outb(inb(port) | (1 << irq_num), port);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {

  uint16_t port;

  /* Check if input is an interrupt on master or slave PIC */
	if((irq_num >= 0) && (irq_num < IRQ_AMNT)) {
		port = MASTER_8259_PORT;
		outb((EOI | irq_num), port); /* Send EOI of to port on Master */
	}
	else {
		port = SLAVE_8259_PORT;
		irq_num %= IRQ_AMNT; /* Re-define number for slave PIC */
		outb((EOI | irq_num), port); /* Send EOI of to port on Slave */

		port = MASTER_8259_PORT;
		outb((EOI | IRQ2), port); /* Send EOI of to connection port on Master */
	}
}
