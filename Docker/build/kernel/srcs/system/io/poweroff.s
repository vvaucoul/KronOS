global poweroff
extern outw

poweroff:
    push ebp

    mov ebx, 0x2000 ; Push last parameter first
    push ebx

    mov edi, 0x604 ; Push first parameter Last
    push edi
    
    call outw ; Call the function

    pop ebx
    pop edi
    pop ebp
    ret