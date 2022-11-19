bits 32

global __asm_kpause
__asm_kpause:
    loop:
        pause
        jmp loop