#define ASM     1


# linkage to IDT
.globl sys_call
.extern sys_call






# jumptable for system calls
    # needs the null for the 0th element
syscall_jumptable:
    .long 0x0, halt, execute, read, write, open, close # , getargs, vidmap, set_handler, sigreturn




sys_call:
    pushal
    pushfl
# pushing args onto stack
    pushl %edx            # Arg 3
    pushl %ecx            # Arg 2
    pushl %ebx            # Arg 1 
# check for valid syscall number
    cmpl $1, %eax
    jb  invalid_syscall
    cmpl $10, %eax
    ja  invalid_syscall
# ??? syscall_jumptable call
    call *syscall_jumptable(, %eax, 4)









# place holder
invalid_syscall:
    orl 0xFFFFFFFF, %eax        # return -1
done:
    popl %ebx                   # restore stack
    popl %ecx
    popl %edx
    popfl       
    popal
    iret







