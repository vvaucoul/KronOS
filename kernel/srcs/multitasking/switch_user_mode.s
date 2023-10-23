global switch_user_mode
switch_user_mode:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23
    push esp
    pushf
    push 0x1b
    lea eax, [switch_user_mode_ret]
    push eax
    iretd

switch_user_mode_ret:
    add esp, 4
    ret
