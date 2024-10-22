bits 32
global enable_paging
enable_paging:
    mov eax, cr0
    or eax, 0x80000000    ; Activer le bit de pagination (bit 31)
    mov cr0, eax          ; Écrire la nouvelle valeur de CR0
    ret