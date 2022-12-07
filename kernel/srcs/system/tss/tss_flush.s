global tss_flush
tss_flush:
    mov eax, [esp + 4]
    ltr [eax]
    ret