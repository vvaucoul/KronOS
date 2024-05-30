global tss_flush
tss_flush:
    mov ax, 0x38  ; TSS selector is 0x38 (index 7 in GDT, multiplied by 8)
    ltr ax
    ret