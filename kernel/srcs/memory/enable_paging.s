%define EAX_REG 0x80000001

;extern page_directory

section .text
global enable_paging

enable_paging:
    mov eax, ebp
    mov cr3, eax

    mov eax, cr0
    or eax, EAX_REG
    mov cr0, eax