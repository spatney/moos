.set IRQ_BASE, 0x20

.section .text

.extern _ZN4moos8hardware16InterruptManager15handleInterruptEhj
.global _ZN16InterruptManager22ignoreInterruptRequestEv

.macro HandleException num
.global _ZN4moos8hardware16InterruptManager16handleInterruptException\num\()Ev
_ZN4moos8hardware16InterruptManager16handleInterruptException\num\()Ev:
    mov $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4moos8hardware16InterruptManager26handleInterruptRequest\num\()Ev
_ZN4moos8hardware16InterruptManager26handleInterruptRequest\num\()Ev:
    mov $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call _ZN4moos8hardware16InterruptManager15handleInterruptEhj
    # addl $5, %esp
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

.global _ZN4moos8hardware16InterruptManager22ignoreInterruptRequestEv
_ZN4moos8hardware16InterruptManager22ignoreInterruptRequestEv:
    iret


.data
    interruptnumber: .byte 0