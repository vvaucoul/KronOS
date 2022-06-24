global poweroff

poweroff:
    mov ah, 42h
    mov bx, 1
    mov cx, 3
    xor bx, bx
    int 15h

    ; mov ax, 5301h
    ; xor bx, bx
    ; int 15h
    
    ; mov ax, 530Fh
    ; mov bx, 1
    ; mov cx, 1
    ; int 15h
    
    ; mov ax, 5307h
    ; mov bx, 1
    ; mov cx, 3
    ; int 15h
