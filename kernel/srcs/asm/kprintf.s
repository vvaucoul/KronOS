bits 32

extern kprintf
extern __asm_kpause

section .data
msg:    db "Hello, world,", 0
fmt:    db "%s", 10, 0

section .text
global __asm_kprintf
__asm_kprintf:
    mov esi, msg
    mov edi, fmt
    mov eax, 0
    call kprintf
    mov eax, 0
    call __asm_kpause
    ret
