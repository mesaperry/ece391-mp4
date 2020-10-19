#ifndef _IDT_H
#define _IDT_H

#include "types.h"







void init_idt();
void provisional_exception();
void provisional_interrupt();
void setup_idt_exceptions();
void setup_idt_interrupts();

#endif  // _IDT_H
