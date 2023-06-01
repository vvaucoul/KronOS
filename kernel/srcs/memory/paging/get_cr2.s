bits 32

global get_cr2
get_cr2:
    mov eax, cr2
    ret