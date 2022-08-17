%define EAX_REG 0x80000001

;extern page_directory

section .text
global enable_paging

enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret