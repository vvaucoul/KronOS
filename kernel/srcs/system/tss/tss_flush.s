global tss_flush
tss_flush:
    mov eax, [esp + 4]
    ltr [eax]
    ret

    ; mov ax, 0x28 ; 0x28 is the TSS segment
    ; ltr ax
    ; ret