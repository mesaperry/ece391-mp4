#define ASM     1
#include "x86_desc.h"
.global jump_userspace, exec_ret
.extern jump_userpac
#define USER_DS 0x002B
#define USER_CS 0x0023


; pushl	%P1							\n\
; pushl	%4							\n\
; pushf									\n\
; pushl	%P2							\n\
; pushl	%3							\n\
; iret									\n\
; exec_ret:							\n\
; movb	%%bl, %0				\n\
; "
; : "=rm"(output)
; : "p"(USER_DS), 
; "p"(USER_CS), 
; "r"(*(uint32_t*)(USER_PROCESS_START_VIRTUAL + ELF_OFFSET)), 
; "r"(virtual_addr)
; : "cc", "memory"
; );
   
#   asm volitile(
#  
#       pushl virtal_addr 
#       pushl eip location
#       jmp jump_userspace
#



# lower addr
#
#
#
# higher addr

jump_userspace:
    mov   USER_DS, %ds
    pushl USER_DS
    movl  8(%ebp), %eax # virtual_addr
    pushl %eax
    pushf
    pushl USER_CS
    movl  4(%ebp), %eax # eip_location
    pushl %eax  
    iret
exec_ret:
    movb %bl, %0



