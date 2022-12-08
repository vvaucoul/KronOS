bits 32
global save_stack
save_stack:
    mov eax, esp
    push eax

    mov eax, [esp + 4]

    pop eax

    ; ret with restoring interrupt flags
    iret