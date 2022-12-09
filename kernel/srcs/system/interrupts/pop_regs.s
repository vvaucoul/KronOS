bits 32

global pop_regs
pop_regs:
    ; Restore the stack pointer
    pop eax
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore the registers
    popa

    ; Restore the stack
    add esp, 0x8

    ; Restore the interrupt flag
    sti
    iret