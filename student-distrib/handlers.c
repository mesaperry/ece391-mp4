#include "handlers.h"
#include "lib.h"
#include "i8259.h"


extern void excpt0_handler()
{
    clear();
    printf("EXCEPTION0: Divide by Zero Error\n");
    while(1);
}

extern void excpt1_handler()
{
    clear();
    printf("EXCEPTION1: Debug\n");
    while(1);
}

extern void excpt2_handler()
{
    clear();
    printf("EXCEPTION2: NMI Interrupt\n");
    while(1);
}

extern void excpt3_handler()
{
    clear();
    printf("EXCEPTION3: Breakpoint");
    while(1);
}

extern void excpt4_handler()
{
    clear();
    printf("EXCEPTION4: Overflow");
    while(1);
}

extern void excpt5_handler()
{
    clear();
    printf("EXCEPTION5: BOUND Range Exceeded");
    while(1);
}

extern void excpt6_handler()
{
    clear();
    printf("EXCEPTION6: Invalid Opcode");
    while(1);
}

extern void excpt7_handler()
{
    clear();
    printf("EXCEPTION7: Device Not Available");
    while(1);
}

extern void excpt8_handler()
{
    clear();
    printf("EXCEPTION8: double fault");
    while(1);
}

extern void excpt9_handler()
{
    clear();
    printf("EXCEPTION9: Coprocessor Segment Overrun");
    while(1);
}

extern void excpt10_handler()
{
    clear();
    printf("EXCEPTION10: Invalid TSS");
    while(1);
}

extern void excpt11_handler()
{
    clear();
    printf("EXCEPTION11: Segment Not Present");
    while(1);
}

extern void excpt12_handler()
{
    clear();
    printf("EXCEPTION12: Stack Fault");
    while(1);
}

extern void excpt13_handler()
{
    clear();
    printf("EXCEPTION13: General Protection");
    while(1);
}

extern void excpt14_handler()
{
    clear();
    printf("EXCEPTION14: Page Fault");
    while(1);
}

extern void excpt15_handler()
{
    clear();
    printf("EXCEPTION15: Reserved for Intel");
    while(1);
}

extern void excpt16_handler()
{
    clear();
    printf("EXCEPTION16: Floating-Point Error");
    while(1);
}

extern void excpt17_handler()
{
    clear();
    printf("EXCEPTION17: Alignment Check");
    while(1);
}

extern void excpt18_handler()
{
    clear();
    printf("EXCEPTION18: Machine Check");
    while(1);
}

extern void excpt19_handler()
{
    clear();
    printf("EXCEPTION19: SIMD Floating-Point");
    while(1);
}

extern void excpt20_handler()
{
    clear();
    printf("EXCEPTION20: Virtualization Exception");
    while(1);
}

extern void excpt21_handler()
{
clear();
printf("EXCEPTION21: Reserved for Intel");
while(1);
}
extern void excpt22_handler()
{
clear();
printf("EXCEPTION22: Reserved for Intel");
while(1);
}
extern void excpt23_handler()
{
clear();
printf("EXCEPTION23: Reserved for Intel");
while(1);
}
extern void excpt24_handler()
{
clear();
printf("EXCEPTION24: Reserved for Intel");
while(1);
}
extern void excpt25_handler()
{
clear();
printf("EXCEPTION25: Reserved for Intel");
while(1);
}
extern void excpt26_handler()
{
clear();
printf("EXCEPTION26: Reserved for Intel");
while(1);
}
extern void excpt27_handler()
{
clear();
printf("EXCEPTION27: Reserved for Intel");
while(1);
}
extern void excpt28_handler()
{
clear();
printf("EXCEPTION28: Reserved for Intel");
while(1);
}
extern void excpt29_handler()
{
clear();
printf("EXCEPTION29: Reserved for Intel");
while(1);
}
extern void excpt30_handler()
{
    clear();
    printf("EXCEPTION30: Security Explination");
    while(1);
}
extern void excpt31_handler()
{
    clear();
    printf("EXCEPTION31: Reserved for Intel");
    while(1);
}

extern void gen_sys_call()
{
    printf("A system call was made\n");
    while(1);
}
/*
extern void irq0_handler(); 
extern void irq1_handler(); 
extern void irq2_handler(); 
extern void irq3_handler(); 
extern void irq4_handler(); 
extern void irq5_handler();
extern void irq6_handler(); 
extern void irq7_handler(); 
extern void irq8_handler(); 
extern void irq9_handler(); 
extern void irq10_handler(); 
extern void irq11_handler();
extern void irq12_handler(); 
extern void irq13_handler(); 
extern void irq14_handler(); 
extern void irq15_handler();
*/