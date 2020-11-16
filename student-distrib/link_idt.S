#define ASM     1
#include "irq_handle.h"

# Assembly link for IDT setup

.text
# setup for irq#
.globl irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15

# handlers for pic_intrs
.extern irq0_handler, irq1_handler, irq2_handler, irq3_handler, irq4_handler, irq5_handler, irq6_handler, irq7_handler, irq8_handler, irq9_handler, irq10_handler, irq11_handler, irq12_handler, irq13_handler, irq14_handler, irq15_handler
.globl irq0_handler, irq1_handler, irq2_handler, irq3_handler, irq4_handler, irq5_handler, irq6_handler, irq7_handler, irq8_handler, irq9_handler, irq10_handler, irq11_handler, irq12_handler, irq13_handler, irq14_handler, irq15_handler
# setup for sys_call functions
# .global sys_call

# .extern gen_sys_call

# setup for keyboard and rtc
.global keyboard_intr, rtc_intr

# handlers for keyboard and rtc
.extern keyboard_handler, rtc_handler

# interrupt handler linkages for keyboard and rtc
# clear intr
# push regs and flags
# call handler
# pop regs and flags
# iret

keyboard_intr:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

rtc_intr:
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    iret

# irq interrupt handler linkage for pic

irq0:
        pushal
        pushfl
        call irq0_handler
        popfl
        popal
        iret
irq1:
        pushal
        pushfl
        call keyboard_handler
        popfl
        popal
        iret
irq2:
        pushal
        pushfl
        call irq2_handler
        popfl
        popal
        iret
irq3:
        pushal
        pushfl
        call irq3_handler
        popfl
        popal
        iret
irq4:
        pushal
        pushfl
        call irq4_handler
        popfl
        popal
        iret
irq5:
        pushal
        pushfl
        call irq5_handler
        popfl
        popal
        iret
irq6:
        pushal
        pushfl
        call irq6_handler
        popfl
        popal
        iret
irq7:
        pushal
        pushfl
        call irq7_handler
        popfl
        popal
        iret
irq8:
        pushal
        pushfl
        call irq8_handler
        popfl
        popal
        iret
irq9:
        pushal
        pushfl
        call irq9_handler
        popfl
        popal
        iret
irq10:
        pushal
        pushfl
        call irq10_handler
        popfl
        popal
        iret
irq11:
        pushal
        pushfl
        call irq11_handler
        popfl
        popal
        iret
irq12:
        pushal
        pushfl
        call irq12_handler
        popfl
        popal
        iret
irq13:
        pushal
        pushfl
        call irq13_handler
        popfl
        popal
        iret
irq14:
        pushal
        pushfl
        call irq14_handler
        popfl
        popal
        iret
irq15:
        pushal
        pushfl
        call irq15_handler
        popfl
        popal
        iret
