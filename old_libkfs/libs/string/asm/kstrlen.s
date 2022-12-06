global kstrlen

kstrlen:
    push ebp
    mov ebp, esp

    mov edx, [ebp+8]
    xor eax, eax

    jmp if

then:
    inc eax

if:
    mov cl, [edx+eax]
    cmp cl, 0x0
    jne then

end:
    pop ebp
    ret