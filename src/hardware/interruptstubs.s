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

HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31

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
    add %esp, 6
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