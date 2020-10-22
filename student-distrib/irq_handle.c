#include "irq_handle.h"
#include "lib.h"

void irq0_handler()
{
        cli();

        printf("Interrupt0:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq1_handler()
{
        cli();
        printf("Interrupt1:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq2_handler()
{
        cli();
        printf("Interrupt2:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq3_handler()
{
        cli();
        printf("Interrupt3:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq4_handler()
{
        cli();
        printf("Interrupt4:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq5_handler()
{
        cli();
        printf("Interrupt5:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq6_handler()
{
        cli();
        printf("Interrupt6:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq7_handler()
{
        cli();
        printf("Interrupt7:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq8_handler()
{
        cli();
        printf("Interrupt8:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq9_handler()
{
        cli();
        printf("Interrupt9:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq10_handler()
{
        cli();
        printf("Interrupt10:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq11_handler()
{
        cli();
        printf("Interrupt11:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq12_handler()
{
        cli();
        printf("Interrupt12:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq13_handler()
{
        cli();
        printf("Interrupt13:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq14_handler()
{
        cli();
        printf("Interrupt14:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}


void irq15_handler()
{
        cli();
        printf("Interrupt15:     interrupt");
        outb(0x20, 0x20); //EOI
        sti();
}

