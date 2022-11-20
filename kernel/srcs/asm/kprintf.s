bits 32

extern printk
extern __asm_kpause

section .data
msg:    db "Hello, world,", 0
fmt:    db "%s", 10, 0

section .text
global __asm_printk
__asm_kprintf:
    mov esi, msg
    mov edi, fmt
    mov eax, 0
    call printk
    mov eax, 0
    call __asm_kpause
    ret
