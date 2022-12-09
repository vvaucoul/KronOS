bits 32

section .text

global push_regs
push_regs:
    ; Save the registers
    pusha

    ; Save the segment registers
    push ds
    push es
    push fs
    push gs

    ; Set the data segment to 0x10
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Save the stack pointer
    mov eax, esp
    push eax
