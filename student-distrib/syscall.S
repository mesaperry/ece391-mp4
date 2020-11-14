#define ASM     1
#include "x86_desc.h"
.text

.globl jump_userspace
# .extern jump_userspace

jump_userspace:
    # set up stackframe
    pushl %ebp
    movl  %esp, %ebp
    movl   USER_DS, %eax    # This is the Page fault
    mov  %ax, %ds
    pushl USER_DS           # This is A Page fault when above is commented
    movl  12(%ebp), %eax  
    pushl %eax
    pushf
    pushl USER_CS
    movl  8(%ebp), %eax  
    pushl %eax  
    iret



