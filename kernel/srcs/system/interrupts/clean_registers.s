bits 32

global clean_registers
clean_registers:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Do stuff

    pop gs
    pop fs
    pop es
    pop ds
    popa

    ret