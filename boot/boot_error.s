%define DEFAULT_MEMORY_PLACEMENT 0x000B8000

section	.data
ERROR_MSG db "Unknown error", 0
FILE_MSG db "File not found", 0

section .text

extern bsod

global display_error_msg
display_error_msg:
    ; Clear the screen
    call __clear_screen

    ; Display the error message
    mov edi, ERROR_MSG
    mov esi, FILE_MSG
    push esi
    push edi
    call bsod
    pop edi
    pop esi
    ret

__clear_screen:
    xor ecx, ecx
    .loop:
        cmp ecx, 80 * 50
        je .ret
        mov dword [DEFAULT_MEMORY_PLACEMENT + ecx], 0x0000
        add ecx, 2
        jmp .loop
    ret
.ret:
    ret