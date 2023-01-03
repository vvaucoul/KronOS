bits 32
global save_stack
save_stack:
    mov eax, esp
    push eax

    mov eax, [esp + 4]
    mov ebx, [esp + 8]

    call ebx

    pop eax

    ; ret with restoring interrupt flags
    iret