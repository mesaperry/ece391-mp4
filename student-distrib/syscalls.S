#define ASM     1






jump_userspace:
    mov   USER_DS, %ds
    pushl USER_DS
    movl  8(%ebp), %eax  
    pushl %eax
    pushf
    pushl USER_CS
    movl  4(%ebp), %eax  
    pushl %eax  
    iret
exec_ret:
    movb %bl, %0



