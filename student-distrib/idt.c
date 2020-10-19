#include "idt.h"
#include "x86_desc.h"
#include "handlers.h"
#include "lib.h"

#define UPPER_MASK 0xffff0000 // selects upper memeory bits
#define SYS_CALL   0x80        

// assembly link declarations
extern void sys_call();

extern void excpt0();
extern void excpt1();
extern void excpt2();
extern void excpt3();
extern void excpt4();
extern void excpt5();
extern void excpt6();
extern void excpt7();
extern void excpt8();
extern void excpt9();
extern void excpt10();
extern void excpt11();
extern void excpt12();
extern void excpt13();
extern void excpt14();
extern void excpt15();
extern void excpt16();
extern void excpt17();
extern void excpt18();
extern void excpt19();
extern void excpt20();
extern void excpt21();
extern void excpt22();
extern void excpt23();
extern void excpt24();
extern void excpt25();
extern void excpt26();
extern void excpt27();
extern void excpt28();
extern void excpt29();
extern void excpt30();
extern void excpt31();

extern void irq0(); 
extern void irq1(); 
extern void irq2(); 
extern void irq3(); 
extern void irq4(); 
extern void irq5();
extern void irq6(); 
extern void irq7(); 
extern void irq8(); 
extern void irq9(); 
extern void irq10(); 
extern void irq11();
extern void irq12(); 
extern void irq13(); 
extern void irq14(); 
extern void irq15();

extern void keybord_intr();
extern void rtc_intr();

void setup_idt_exceptions();
void setup_idt_exceptions();
void setup_idt_interrupts();
void provisional_interrupt();
void provisional_exception();

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
*   IN:  None
*   OUT: None
*   Description: initializes the idt with values
*   Side Effects: The kernel is initailized with a filled IDT and the idt ptr is given to the processor
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
        if(irq_n == SYS_CALL) {idt[irq_n].dpl = 0x3;}
        else
        {
            idt[irq_n].dpl = 0x0;
        }    
        
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
        
        // if its 0x80 its a system call
        if (irq_n == SYS_CALL)
        {
            SET_IDT_ENTRY(idt[SYS_CALL], sys_call);
            continue;
        }
        
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
    // after all entries are set up and provisionals are placed in 
    //     fill the actual entreis 
    setup_idt_exceptions();
    setup_idt_interrupts();

    // set up keyboard and rtc
    //SET_IDT_ENTRY(idt[])
}

////INTERRUPTS AND EXCEPTIONS? WHERE DO I GET THESE/what do i print

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
    
    SET_IDT_ENTRY(idt[0], excpt0);          // divide_by_zero 
    SET_IDT_ENTRY(idt[1], excpt1);          // Debug
    SET_IDT_ENTRY(idt[2], excpt2);          // NMI Interrupt
    SET_IDT_ENTRY(idt[3], excpt3);          // Breakpoint
    SET_IDT_ENTRY(idt[4], excpt4);          // Overflow
    SET_IDT_ENTRY(idt[5], excpt5);          // BOUND Range Exceeded 
    SET_IDT_ENTRY(idt[6], excpt6);          // Invalid Opcode
    SET_IDT_ENTRY(idt[7], excpt7);          // Device Not Available
    SET_IDT_ENTRY(idt[8], excpt8);          // Double Fault
    SET_IDT_ENTRY(idt[9], excpt9);          // Coprocessor Segment Overrun
    SET_IDT_ENTRY(idt[10], excpt10);        // Invalid TSS
    SET_IDT_ENTRY(idt[11], excpt11);        // Segment Not Present
    SET_IDT_ENTRY(idt[12], excpt12);        // Stack Fault
    SET_IDT_ENTRY(idt[13], excpt13);        // General Protection
    SET_IDT_ENTRY(idt[14], excpt14);        // Page Fault
    SET_IDT_ENTRY(idt[15], excpt15);        // Reserved for Intel
    SET_IDT_ENTRY(idt[16], excpt16);        // Floating-Point Error
    SET_IDT_ENTRY(idt[17], excpt17);        // Alignment Check
    SET_IDT_ENTRY(idt[18], excpt18);        // Machine Check
    SET_IDT_ENTRY(idt[19], excpt19);        // SIMD floating-point
    SET_IDT_ENTRY(idt[20], excpt20);
    SET_IDT_ENTRY(idt[21], excpt21);        // 21-19 Reserved for Intel
    SET_IDT_ENTRY(idt[22], excpt22);
    SET_IDT_ENTRY(idt[23], excpt23);
    SET_IDT_ENTRY(idt[24], excpt24);
    SET_IDT_ENTRY(idt[25], excpt25);
    SET_IDT_ENTRY(idt[26], excpt26);
    SET_IDT_ENTRY(idt[27], excpt27);
    SET_IDT_ENTRY(idt[28], excpt28);
    SET_IDT_ENTRY(idt[29], excpt29);
    SET_IDT_ENTRY(idt[30], excpt30);         // Securtiy Explination
    SET_IDT_ENTRY(idt[31], excpt31);         // Reserved for Intel
    
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
    
    SET_IDT_ENTRY(idt[32], irq0);
    SET_IDT_ENTRY(idt[33], irq1);
    SET_IDT_ENTRY(idt[34], irq2);
    SET_IDT_ENTRY(idt[35], irq3);
    SET_IDT_ENTRY(idt[36], irq4);
    SET_IDT_ENTRY(idt[37], irq5);
    SET_IDT_ENTRY(idt[38], irq6);
    SET_IDT_ENTRY(idt[39], irq7);
    SET_IDT_ENTRY(idt[40], irq8);
    SET_IDT_ENTRY(idt[41], irq9);
    SET_IDT_ENTRY(idt[42], irq10);
    SET_IDT_ENTRY(idt[43], irq11);
    SET_IDT_ENTRY(idt[44], irq12);
    SET_IDT_ENTRY(idt[45], irq13);
    SET_IDT_ENTRY(idt[46], irq14);
    SET_IDT_ENTRY(idt[47], irq15);
}

