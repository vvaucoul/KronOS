bits 32

section .text
%macro ISR_NO_ERROR 1
global isr%1

isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_exception_handler
%endmacro

%macro ISR_ERROR 1
global isr%1

isr%1:
    cli
    push byte %1
    jmp isr_exception_handler
%endmacro

ISR_NO_ERROR  0
ISR_NO_ERROR  1
ISR_NO_ERROR  2
ISR_NO_ERROR  3
ISR_NO_ERROR  4
ISR_NO_ERROR  5
ISR_NO_ERROR  6
ISR_NO_ERROR  7
ISR_ERROR     8
ISR_NO_ERROR  9
ISR_ERROR    10
ISR_ERROR    11
ISR_ERROR    12
ISR_ERROR    13
ISR_ERROR    14
ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_ERROR    17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_ERROR    21
ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28
ISR_NO_ERROR 29
ISR_NO_ERROR 30
ISR_NO_ERROR 31

extern fault_handler

isr_exception_handler:

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

    ; Call the fault handler
    mov eax, fault_handler
    call eax

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