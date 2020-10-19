#include "handlers.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"

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
    clear();
    printf("EXCEPTION0: Divide by Zero Error\n");
    while(1);
}

void excpt1_handler()
{
    clear();
    printf("EXCEPTION1: Debug\n");
    while(1);
}

void excpt2_handler()
{
    clear();
    printf("EXCEPTION2: NMI Interrupt\n");
    while(1);
}

void excpt3_handler()
{
    clear();
    printf("EXCEPTION3: Breakpoint");
    while(1);
}

void excpt4_handler()
{
    clear();
    printf("EXCEPTION4: Overflow");
    while(1);
}

void excpt5_handler()
{
    clear();
    printf("EXCEPTION5: BOUND Range Exceeded");
    while(1);
}

void excpt6_handler()
{
    clear();
    printf("EXCEPTION6: Invalid Opcode");
    while(1);
}

void excpt7_handler()
{
    clear();
    printf("EXCEPTION7: Device Not Available");
    while(1);
}

void excpt8_handler()
{
    clear();
    printf("EXCEPTION8: double fault");
    while(1);
}

void excpt9_handler()
{
    clear();
    printf("EXCEPTION9: Coprocessor Segment Overrun");
    while(1);
}

void excpt10_handler()
{
    clear();
    printf("EXCEPTION10: Invalid TSS");
    while(1);
}

void excpt11_handler()
{
    clear();
    printf("EXCEPTION11: Segment Not Present");
    while(1);
}

void excpt12_handler()
{
    clear();
    printf("EXCEPTION12: Stack Fault");
    while(1);
}

void excpt13_handler()
{
    clear();
    printf("EXCEPTION13: General Protection");
    while(1);
}

void excpt14_handler()
{
    clear();
    printf("EXCEPTION14: Page Fault");
    while(1);
}

void excpt15_handler()
{
    clear();
    printf("EXCEPTION15: Reserved for Intel");
    while(1);
}

void excpt16_handler()
{
    clear();
    printf("EXCEPTION16: Floating-Point Error");
    while(1);
}

void excpt17_handler()
{
    clear();
    printf("EXCEPTION17: Alignment Check");
    while(1);
}

void excpt18_handler()
{
    clear();
    printf("EXCEPTION18: Machine Check");
    while(1);
}

void excpt19_handler()
{
    clear();
    printf("EXCEPTION19: SIMD Floating-Point");
    while(1);
}

void excpt20_handler()
{
    clear();
    printf("EXCEPTION20: Virtualization Exception");
    while(1);
}

void excpt21_handler()
{
clear();
printf("EXCEPTION21: Reserved for Intel");
while(1);
}
void excpt22_handler()
{
clear();
printf("EXCEPTION22: Reserved for Intel");
while(1);
}
void excpt23_handler()
{
clear();
printf("EXCEPTION23: Reserved for Intel");
while(1);
}
void excpt24_handler()
{
clear();
printf("EXCEPTION24: Reserved for Intel");
while(1);
}
void excpt25_handler()
{
clear();
printf("EXCEPTION25: Reserved for Intel");
while(1);
}
void excpt26_handler()
{
clear();
printf("EXCEPTION26: Reserved for Intel");
while(1);
}
void excpt27_handler()
{
clear();
printf("EXCEPTION27: Reserved for Intel");
while(1);
}
void excpt28_handler()
{
clear();
printf("EXCEPTION28: Reserved for Intel");
while(1);
}
void excpt29_handler()
{
clear();
printf("EXCEPTION29: Reserved for Intel");
while(1);
}
void excpt30_handler()
{
    clear();
    printf("EXCEPTION30: Security Explination");
    while(1);
}
void excpt31_handler()
{
    clear();
    printf("EXCEPTION31: Reserved for Intel");
    while(1);
}

void gen_sys_call(){
    printf("A system call was made\n");
    while(1);
}
/*
void irq0_handler(); 
void irq1_handler(); 
void irq2_handler(); 
void irq3_handler(); 
void irq4_handler(); 
void irq5_handler();
void irq6_handler(); 
void irq7_handler(); 
void irq8_handler(); 
void irq9_handler(); 
void irq10_handler(); 
void irq11_handler();
void irq12_handler(); 
void irq13_handler(); 
void irq14_handler(); 
void irq15_handler();
*/