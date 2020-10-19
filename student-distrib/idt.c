
#include "idt.h"

#include "x86_desc.h"
#include "lib.h"

#define UPPER_MASK 0xffff0000 // selects upper memeory bits
#define SYS_CALL   0x80        
#define KEYBOARD_VEC 0x60
#define RTC_VEC      0x70
// assembly link declarations

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

void sys_call();
void excpt0_handler();
void excpt1_handler();
void excpt2_handler();
void excpt3_handler();
void excpt4_handler();
void excpt5_handler();
void excpt6_handler();
void excpt7_handler();
void excpt8_handler();
void excpt9_handler();
void excpt10_handler();
void excpt11_handler();
void excpt12_handler();
void excpt13_handler();
void excpt14_handler();
void excpt15_handler();
void excpt16_handler();
void excpt17_handler();
void excpt18_handler();
void excpt19_handler();
void excpt20_handler();
void excpt21_handler();
void excpt22_handler();
void excpt23_handler();
void excpt24_handler();
void excpt25_handler();
void excpt26_handler();
void excpt27_handler();
void excpt28_handler();
void excpt29_handler();
void excpt30_handler();
void excpt31_handler();

// @TODO fix extern and assembly file

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
      /*  if (irq_n == SYS_CALL)
        {
            
            continue;
        }*/
        
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
    SET_IDT_ENTRY(idt[SYS_CALL], sys_call);
    //set up keyboard and rtc
    SET_IDT_ENTRY(idt[KEYBOARD_VEC], keybord_intr);
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_intr);
}

////INTERRUPTS AND EXCEPTIONS? WHERE DO I GET THESE/what do i print

/*
*   except#_handler() 
*   IN: None
*   OUT: exception message
*   Description: Handler for exception, prints exception message and loops
*   Side Effects: Prints to terminal and loops
*
*/

void excpt0_handler()
{
    
    printf("EXCEPTION0: Divide by Zero Error\n");
    while(1);
}

void excpt1_handler()
{
    
    printf("EXCEPTION1: Debug\n");
    while(1);
}

void excpt2_handler()
{
    
    printf("EXCEPTION2: NMI Interrupt\n");
    while(1);
}

void excpt3_handler()
{
    
    printf("EXCEPTION3: Breakpoint");
    while(1);
}

void excpt4_handler()
{
    
    printf("EXCEPTION4: Overflow");
    while(1);
}

void excpt5_handler()
{
    
    printf("EXCEPTION5: BOUND Range Exceeded");
    while(1);
}

void excpt6_handler()
{
    
    printf("EXCEPTION6: Invalid Opcode");
    while(1);
}

void excpt7_handler()
{
    
    printf("EXCEPTION7: Device Not Available");
    while(1);
}

void excpt8_handler()
{
    
    printf("EXCEPTION8: double fault");
    while(1);
}

void excpt9_handler()
{
    
    printf("EXCEPTION9: Coprocessor Segment Overrun");
    while(1);
}

void excpt10_handler()
{
    
    printf("EXCEPTION10: Invalid TSS");
    while(1);
}

void excpt11_handler()
{
    
    printf("EXCEPTION11: Segment Not Present");
    while(1);
}

void excpt12_handler()
{
    
    printf("EXCEPTION12: Stack Fault");
    while(1);
}

void excpt13_handler()
{
    
    printf("EXCEPTION13: General Protection");
    while(1);
}

void excpt14_handler()
{
    
    printf("EXCEPTION14: Page Fault");
    while(1);
}

void excpt15_handler()
{
    
    printf("EXCEPTION15: Reserved for Intel");
    while(1);
}

void excpt16_handler()
{
    
    printf("EXCEPTION16: Floating-Point Error");
    while(1);
}

void excpt17_handler()
{
    
    printf("EXCEPTION17: Alignment Check");
    while(1);
}

void excpt18_handler()
{
    
    printf("EXCEPTION18: Machine Check");
    while(1);
}

void excpt19_handler()
{
    
    printf("EXCEPTION19: SIMD Floating-Point");
    while(1);
}

void excpt20_handler()
{
    
    printf("EXCEPTION20: Virtualization Exception");
    while(1);
}

void excpt21_handler()
{
    
    printf("EXCEPTION21: Reserved for Intel");
    while(1);
}

void excpt22_handler()
{
    
    printf("EXCEPTION22: Reserved for Intel");
    while(1);   
}

void excpt23_handler()
{
    
    printf("EXCEPTION23: Reserved for Intel");
    while(1);
}

void excpt24_handler()
{
    
    printf("EXCEPTION24: Reserved for Intel");
    while(1);
}

void excpt25_handler()
{
    
    printf("EXCEPTION25: Reserved for Intel");
    while(1);
}

void excpt26_handler()
{
    
    printf("EXCEPTION26: Reserved for Intel");
    while(1);
}

void excpt27_handler()
{
    
    printf("EXCEPTION27: Reserved for Intel");
    while(1);
}

void excpt28_handler()
{
    
    printf("EXCEPTION28: Reserved for Intel");
    while(1);
}

void excpt29_handler()
{
    
    printf("EXCEPTION29: Reserved for Intel");
    while(1);
}

void excpt30_handler()
{
    
    printf("EXCEPTION30: Security Explination");
    while(1);
}

void excpt31_handler()
{
    
    printf("EXCEPTION31: Reserved for Intel");
    while(1);
}

void sys_call(){
    printf("A system call was made\n");
    while(1);
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
    
    SET_IDT_ENTRY(idt[0], excpt0_handler);          // divide_by_zero 
    SET_IDT_ENTRY(idt[1], excpt1_handler);          // Debug
    SET_IDT_ENTRY(idt[2], excpt2_handler);          // NMI Interrupt
    SET_IDT_ENTRY(idt[3], excpt3_handler);          // Breakpoint
    SET_IDT_ENTRY(idt[4], excpt4_handler);          // Overflow
    SET_IDT_ENTRY(idt[5], excpt5_handler);          // BOUND Range Exceeded 
    SET_IDT_ENTRY(idt[6], excpt6_handler);          // Invalid Opcode
    SET_IDT_ENTRY(idt[7], excpt7_handler);          // Device Not Available
    SET_IDT_ENTRY(idt[8], excpt8_handler);          // Double Fault
    SET_IDT_ENTRY(idt[9], excpt9_handler);          // Coprocessor Segment Overrun
    SET_IDT_ENTRY(idt[10], excpt10_handler);        // Invalid TSS
    SET_IDT_ENTRY(idt[11], excpt11_handler);        // Segment Not Present
    SET_IDT_ENTRY(idt[12], excpt12_handler);        // Stack Fault
    SET_IDT_ENTRY(idt[13], excpt13_handler);        // General Protection
    SET_IDT_ENTRY(idt[14], excpt14_handler);        // Page Fault
    SET_IDT_ENTRY(idt[15], excpt15_handler);        // Reserved for Intel
    SET_IDT_ENTRY(idt[16], excpt16_handler);        // Floating-Point Error
    SET_IDT_ENTRY(idt[17], excpt17_handler);        // Alignment Check
    SET_IDT_ENTRY(idt[18], excpt18_handler);        // Machine Check
    SET_IDT_ENTRY(idt[19], excpt19_handler);        // SIMD floating-point
    SET_IDT_ENTRY(idt[20], excpt20_handler);
    SET_IDT_ENTRY(idt[21], excpt21_handler);        // 21-19 Reserved for Intel
    SET_IDT_ENTRY(idt[22], excpt22_handler);
    SET_IDT_ENTRY(idt[23], excpt23_handler);
    SET_IDT_ENTRY(idt[24], excpt24_handler);
    SET_IDT_ENTRY(idt[25], excpt25_handler);
    SET_IDT_ENTRY(idt[26], excpt26_handler);
    SET_IDT_ENTRY(idt[27], excpt27_handler);
    SET_IDT_ENTRY(idt[28], excpt28_handler);
    SET_IDT_ENTRY(idt[29], excpt29_handler);
    SET_IDT_ENTRY(idt[30], excpt30_handler);         // Securtiy Explination
    SET_IDT_ENTRY(idt[31], excpt31_handler);         // Reserved for Intel
    
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

