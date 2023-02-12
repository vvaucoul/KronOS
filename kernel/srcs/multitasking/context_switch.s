bits 32

global context_switch
context_switch:
    mov ebx, [esp + 8] ; second argument, the stack frame pointer
    mov eax, [esp + 4] ; first argument, the stack pointer

    mov ebp, ebx ; restore the stack frame
    mov esp, eax ; restore the top of the stack

    popad ; restore all registers

    sti ; OK for interupts at this point
    iretd ; perform a return from interrupt since the last task was interrupted
    ; mov eax, [esp + 4]
    ; mov edx, [esp + 8]

    ; push ebp
    ; push ebx
    ; push esi
    ; push edi

    ; ; maybe use ebp / esp for stack frame
    ; ; mov ebp, eax
    ; ; mov esp, edx
    ; mov eax, esp
    ; mov esp, edx

    ; pop edi
    ; pop esi
    ; pop ebx
    ; pop ebp

    ; ret

global context_switch_jmp
context_switch_jmp:
    mov ebx, [esp + 8] ; second argument, the function pointer
    mov eax, [esp + 4] ; first argument, a location to build a stack
    mov esp, eax ; set the stack pointer to the location
    mov ebp, esp ; by setting the EBP to the ESP, we configure a new stack
    sti ; enable interrupts
    jmp ebx ; jump to the function pointer

    ; mov eax, [esp + 4]
    ; mov edx, [esp + 8]

    ; push ebp
    ; push ebx
    ; push esi
    ; push edi

    ; ; maybe use ebp / esp for stack frame
    ; ; mov ebp, eax
    ; ; mov esp, edx
    ; mov eax, esp
    ; mov esp, edx

    ; pop edi
    ; pop esi
    ; pop ebx
    ; pop ebp

    ; sti
    ; jmp eax