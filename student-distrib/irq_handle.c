#include "irq_handle.h"
#include "lib.h"

void irq0_handler()
{
        cli();

        printf("Interrupt0: System timer");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq1_handler()
{
        cli();
        printf("Interrupt1: Keyboard controller");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq2_handler()
{
        cli();
        printf("Interrupt2: Slave cascade");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq3_handler()
{
        cli();
        printf("Interrupt3: Serial port 2 controller");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq4_handler()
{
        cli();
        printf("Interrupt4: Serial port 1 controller");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq5_handler()
{
        cli();
        printf("Interrupt5: Parallel port 2/3 OR sound card");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq6_handler()
{
        cli();
        printf("Interrupt6: Floppy disk controller");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq7_handler()
{
        cli();
        printf("Interrupt7: Parallel port 1");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq8_handler()
{
        cli();
        printf("Interrupt8: RTC interrupt");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq9_handler()
{
        cli();
        printf("Interrupt9: ACPI system control interrupt");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq10_handler()
{
        cli();
        printf("Interrupt10: Open interrupt for peripherals");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq11_handler()
{
        cli();
        printf("Interrupt11: Open interrupt for peripherals");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq12_handler()
{
        cli();
        printf("Interrupt12: Mouse");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq13_handler()
{
        cli();
        printf("Interrupt13: Inter-processor interupt");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq14_handler()
{
        cli();
        printf("Interrupt14: Primary ATA channel");
        outb(0x20, 0x20); //EOI
        //sti();
}


void irq15_handler()
{
        cli();
        printf("Interrupt15: Secondary ATA channel");
        outb(0x20, 0x20); //EOI
        //sti();
}
