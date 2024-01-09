global switch_user_mode
switch_user_mode:
    cli

    ; Data segment selector = 0x2B (GDT: 5th segment)
    ; 5 * 8 = 40 = 0x28 + 3 (RPL) = 0x2B
    ; (RPL = 3)
    ; RPL: request privilege level (Ring 3)
    mov ax, 0x2B
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x2B
    push esp
    pushf
    
    ; RPL = 3
    push 0x1b
    lea eax, [switch_user_mode_ret]
    push eax
    iretd

switch_user_mode_ret:
    add esp, 4
    ret
