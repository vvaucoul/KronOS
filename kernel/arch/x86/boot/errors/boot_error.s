%define __VGA_MEMORY__ 0x000B8000
%define __VGA_MEMORY_HHK__ 0xC00B8000
IS_HIGHER_HALF_KERNEL equ 1

section .text

extern bsod

global display_error_msg
display_error_msg:
    mov edi, __VGA_MEMORY__

    ; Clear the screen
    mov ecx, IS_HIGHER_HALF_KERNEL
    cmp ecx, 0
    je __clear_screen
    jne __clear_screen_higher
    ret

__display_bsod:
    call bsod
    ret

__clear_screen_higher:
    mov edi, __VGA_MEMORY_HHK__
    jmp __clear_screen

__clear_screen:
    xor esi, esi

    .loop:
        cmp esi, 80 * 50
        je .ret
        mov dword [edi + esi], 0x0000
        add esi, 2
        jmp .loop
    jmp .ret
.ret:
    call __display_bsod
    ret