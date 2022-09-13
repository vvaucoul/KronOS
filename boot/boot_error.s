%define DEFAULT_MEMORY_PLACEMENT 0x000B8000

section	.data
ERROR_MSG db "Error: ", 0x0d, 0x0a, 0

section .text

extern bsod

global display_error_msg
display_error_msg:
    ; Display string
    ; call __clear_screen

    ; Display error message
    mov eax, dword [ERROR_MSG]
    push eax
    call bsod
    pop eax
    ret

__clear_screen:
    xor ecx, ecx
    .loop:
        cmp ecx, 80 * 50
        je .ret
        mov dword [DEFAULT_MEMORY_PLACEMENT + ecx], 0x0000
        add ecx, 2
        jmp .loop
.ret:
    ret