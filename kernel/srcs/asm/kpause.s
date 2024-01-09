bits 32

global __asm_kpause
__asm_kpause:
    cmp   byte [esp+4], 0 ; Compare the byte pointed to by ESP + 4 with zero
    jz    end             ; If the comparison was equal, jump to the end
    mov   eax, 1          ; Otherwise, set EAX to 1
    cpuid                  ; Execute CPUID
    end:
        ret               ; Return from the function
