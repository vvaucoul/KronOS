bits 32

global task_switch_jmp
task_switch_jmp:
    mov eax, [esp + 4]
    mov ebx, [esp + 8]
    
    mov ebp, ebx
    mov esp, eax

    popad

    sti
    jmp ebx

global task_switch
task_switch:
    mov eax, [esp + 4]
    mov ebx, [esp + 8]
    
    mov ebp, ebx
    mov esp, eax

    popad

    sti
    iretd