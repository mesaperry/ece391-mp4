#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

#define UPPER_MASK 0xffff0000 // selects upper memeory bits
#ifndef SYS_CALL   0x80        

#define SYS_CALL_INT 

/*
*   provisional_interrupt
*   IN: None 
*   OUT: message
*   Description: a provisional empty interupt for init idt
*   Side Effects: clears and restores interrupts and prints a message
*/
void provisional_interrupt()
{
    cli();
    printf("provisional interrupt: EMPTY");
    sti();
}


/*
*   provisional_exception
*   IN: None 
*   OUT: message
*   Description: a provisional empty exception for init idt
*   Side Effects: prints a message and gives a while(1) loop
*/
void provisional_exception()
{
    printf("provisional exception: EMPTY");
    while(1);
}



/*
    idt entry : Interrupt Gate
    [31-      Offset      -16-15- P -14- DPL -12- 0 D 1 1 0/1 -8-7- 0 0 0 - 4:0]
    offset_31_16                present  dpl    r0,size,r1,r2,3    r4->
---------------------------------------------------------------------------------
    [31- Segment Selector -16-15- Seg Offset -0]
    seg_selector                offset_15_00
*/



/*
*   init_idt()
*   IN:
*   OUT:
*   Description:
*   Side Effects:
*/
void init_idt()
{
    int irq_n;
    // initalize idt ptr to idtr
    lidt(idt_desc_ptr);
    

    // first fill the idt with blank gates (provisional) 
    for (irq_n = 0; irq_n < NUM_VEC; irq_n++)
    {
        
        // mark the entry as in the memeory
        idt[irq_n].present = 0x1;
        
        // if the intr is a sys call mark it as Desc Power Lvl 3, else mark it as kernel
        if(irq_n == SYS_CALL_INT) {idt[irq_n].dpl = 0x3;}
        idt[irq_n].dpl = 0x0;
        
        // set the reserved bit
        idt[irq_n].reserved0 = 0x0;
        
        // set the size bit to 1 for 32-bit instructions
        idt[irq_n].size = 0x1; 

        idt[irq_n].reserved1 = 0x1;
        idt[irq_n].reserved2 = 0x1;

        // set to 0 for not intel defined vector 
        idt[irq_n].reserved3 = 0x0;
        idt[irq_n].reserved4 = 0x0;

        // set the segment selector
        idt[irq_n].seg_selector = KERNEL_CS;
        
        // if an exception set the correct gate bit and set the provisional entry
        if (irq_n < 32)
        {
            idt[irq_n].reserved3 = 0x1;
            SET_IDT_ENTRY(idt[irq_n], provisional_exception);
        }
        else{
            SET_IDT_ENTRY(idt[irq_n], provisional_interrupt);
        }
    }
    setup_idt_exceptions();
    setup_idt_interrupts();
}


/* @TODO ---------------
*   setup_idt_exceptions()
*   IN:  None
*   OUT: None
*   Description: Initializes execeptions into idt 
*   Side Effects: IDT is filled with exception vectors
*/
void setup_idt_exceptions()
{
    ///SET_IDT_ENTRY(idt[actual_int_num], exception());
    // for all exceptions
}


/* @TODO ---------------
*   setup_idt_interrupts()
*   IN:  None
*   OUT: None
*   Description: Initializes interrupts into idt
*   Side Effects: IDT is filled with interrupt vectors
*/
void setup_idt_interrupts()
{
    ///SET_IDT_ENTRY(idt[actual_int_num], interrupt());
    // for all interrupts
    // starts at x20 and has 16 entries
    //SET_IDT_ENTRY(idt[32], intr);
    //SET_IDT_ENTRY(idt[33], intr);
    //SET_IDT_ENTRY(idt[34], intr);
    //SET_IDT_ENTRY(idt[35], intr);
    //SET_IDT_ENTRY(idt[36], intr);
    //SET_IDT_ENTRY(idt[37], intr);
    //SET_IDT_ENTRY(idt[38], intr);
    //SET_IDT_ENTRY(idt[39], intr);
    //SET_IDT_ENTRY(idt[40], intr);
    //SET_IDT_ENTRY(idt[41], intr);
    //SET_IDT_ENTRY(idt[42], intr);
    //SET_IDT_ENTRY(idt[43], intr);
    //SET_IDT_ENTRY(idt[44], intr);
    //SET_IDT_ENTRY(idt[45], intr);
    //SET_IDT_ENTRY(idt[46], intr);
    //SET_IDT_ENTRY(idt[47], intr);
}

