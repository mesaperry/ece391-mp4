#define ASM     1
#include "handlers.h"
# #include "handlers.c"

# Assembly link for IDT setup

.text

# setup for exception#
.globl excpt0, excpt1, excpt2, excpt3, excpt4, excpt5, excpt6, excpt7, excpt8, excpt9, excpt10, excpt11, excpt12, excpt13, excpt14, excpt15, excpt16, excpt17, excpt18, excpt19, excpt20, excpt21, excpt22, excpt23, excpt24, excpt25, excpt26, excpt27, excpt28, excpt29, excpt30, excpt31

# setup for sys_call functions
.globl sys_call

.extern gen_sys_call

# handlers for exceptions
.extern excpt0_handler, excpt1_handler, excpt2_handler, excpt3_handler, excpt4_handler, excpt5_handler, excpt6_handler, excpt7_handler, excpt8_handler, excpt9_handler, excpt10_handler, excpt11_handler, excpt12_handler, excpt13_handler, excpt14_handler, excpt15_handler, excpt16_handler, excpt17_handler, excpt18_handler, excpt19_handler, excpt20_handler, excpt21_handler, excpt22_handler, excpt23_handler, excpt24_handler, excpt25_handler, excpt26_handler, excpt27_handler, excpt28_handler, excpt29_handler, excpt30_handler, excpt31_handler
.globl excpt0_handler, excpt1_handler, excpt2_handler, excpt3_handler, excpt4_handler, excpt5_handler, excpt6_handler, excpt7_handler, excpt8_handler, excpt9_handler, excpt10_handler, excpt11_handler, excpt12_handler, excpt13_handler, excpt14_handler, excpt15_handler, excpt16_handler, excpt17_handler, excpt18_handler, excpt19_handler, excpt20_handler, excpt21_handler, excpt22_handler, excpt23_handler, excpt24_handler, excpt25_handler, excpt26_handler, excpt27_handler, excpt28_handler, excpt29_handler, excpt30_handler, excpt31_handler

# setup for keyboard and rtc
.globl keyboard_intr, rtc_intr

# handlers for keyboard and rtc
.extern keyboard_handler, rtc_handler

# interrupt handler linkages for keyboard and rtc
# clear intr
# push regs and flags
# call handler
# pop regs and flags
# iret

keyboard_intr:
    cli
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    sti
    iret

rtc_intr:
    cli
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    sti
    iret

# interrupt handler linkages
# push regs and flags
# call handler
# pop regs and flags
# iret
excpt0:
    pushal
    pushfl
    call excpt0_handler
    popfl
    popal
    iret

excpt1:
    pushal
    pushfl
    call excpt1_handler
    popfl
    popal
    iret

excpt2:
    pushal
    pushfl
    call excpt2_handler
    popfl
    popal
    iret

excpt3:
    pushal
    pushfl
    call excpt3_handler
    popfl
    popal
    iret

excpt4:
    pushal
    pushfl
    call excpt4_handler
    popfl
    popal
    iret

excpt5:
    pushal
    pushfl
    call excpt5_handler
    popfl
    popal
    iret

excpt6:
    pushal
    pushfl
    call excpt6_handler
    popfl
    popal
    iret

excpt7:
    pushal
    pushfl
    call excpt7_handler
    popfl
    popal
    iret

excpt8:
    pushal
    pushfl
    call excpt8_handler
    popfl
    popal
    iret

excpt9:
    pushal
    pushfl
    call excpt9_handler
    popfl
    popal
    iret

excpt10:
    pushal
    pushfl
    call excpt10_handler
    popfl
    popal
    iret

excpt11:
    pushal
    pushfl
    call excpt11_handler
    popfl
    popal
    iret

excpt12:
    pushal
    pushfl
    call excpt12_handler
    popfl
    popal
    iret

excpt13:
    pushal
    pushfl
    call excpt13_handler
    popfl
    popal
    iret

excpt14:
    pushal
    pushfl
    call excpt14_handler
    popfl
    popal
    iret

excpt15:
    pushal
    pushfl
    call excpt15_handler
    popfl
    popal
    iret

excpt16:
    pushal
    pushfl
    call excpt16_handler
    popfl
    popal
    iret

excpt17:
    pushal
    pushfl
    call excpt17_handler
    popfl
    popal
    iret

excpt18:
    pushal
    pushfl
    call excpt18_handler
    popfl
    popal
    iret
    
excpt19:
    pushal
    pushfl
    call excpt19_handler
    popfl
    popal
    iret

excpt20:
    pushal
    pushfl
    call excpt20_handler
    popfl
    popal
    iret
    
excpt21:
    pushal
    pushfl
    call excpt21_handler
    popfl
    popal
    iret

excpt22:
    pushal
    pushfl
    call excpt22_handler
    popfl
    popal
    iret

excpt23:
    pushal
    pushfl
    call excpt23_handler
    popfl
    popal
    iret

excpt24:
    pushal
    pushfl
    call excpt24_handler
    popfl
    popal
    iret

excpt25:
    pushal
    pushfl
    call excpt25_handler
    popfl
    popal
    iret

excpt26:
    pushal
    pushfl
    call excpt26_handler
    popfl
    popal
    iret
    
excpt27:
    pushal
    pushfl
    call excpt27_handler
    popfl
    popal
    iret

excpt28:
    pushal
    pushfl
    call excpt28_handler
    popfl
    popal
    iret

excpt29:
    pushal
    pushfl
    call excpt29_handler
    popfl
    popal
    iret

excpt30:
    pushal
    pushfl
    call excpt30_handler
    popfl
    popal
    iret

excpt31:
    pushal
    pushfl
    call excpt31_handler
    popfl
    popal
    iret

sys_call:
    pushal
    pushfl
    call gen_sys_call
    popfl
    popal
    iret


# setup for irq#
# .global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15

# handlers for pic_intrs
# .extern irq0_handler, irq1_handler, irq2_handler, irq3_handler, irq4_handler, irq5_handler, irq6_handler, irq7_handler, irq8_handler, irq9_handler, irq10_handler, irq11_handler, irq12_handler, irq13_handler, irq14_handler, irq15_handler
