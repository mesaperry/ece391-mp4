#define ASM     1

.text

.globl jump_userspace
# .extern jump_userspace

jump_userspace:
    # set up stackframe
    mov   USER_DS, %ds    
    pushl USER_DS
    movl  12(%ebp), %eax  
    pushl %eax
    pushf
    pushl USER_CS
    movl  8(%ebp), %eax  
    pushl %eax  
    iret




; func stack ...
; RA
; virtual_addr
; eip

; linker error : cannot find the symbol definition
; undefined reference to jump_userspace