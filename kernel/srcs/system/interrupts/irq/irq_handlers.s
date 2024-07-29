bits 32

section .text

%macro IRQ 1
global irq%1

irq%1:
	cli
	push byte %1
	push byte 32+%1
	jmp __irq_handler
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

extern irq_handler

__irq_handler:
	; save registers
    pusha
    push ds
    push es
    push fs
    push gs

	; set data segment
    mov ax, 0x10

	; set all segments to data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax

	; call irq handler
    mov eax, irq_handler
    call eax

	; restore registers
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa

	; send EOI
    add esp, 8
    iret
