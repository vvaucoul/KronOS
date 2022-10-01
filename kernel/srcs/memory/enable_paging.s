EAX_REG equ 0x80000001

bits 32

section .text
global enable_paging

enable_paging:
    ; enable paging with cr0
    mov eax, cr0
    or eax, EAX_REG
    mov cr0, eax