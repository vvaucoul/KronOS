%define __NR_open 5
%define __NR_openat 295

bits 32

%macro _syscall0 1
    global %1
    %1:
        push ebp
        mov ebp, esp
        sub esp, 4
        mov eax, __NR_%1
        int 0x80
        mov dword [ebp-4], eax
        cmp eax, 0
        jge .syscall_return
        mov eax, -1
    .syscall_return:
        mov esp, ebp
        pop ebp
        ret
%endmacro

%macro _syscall 3
    global %1
    %1:
        push ebp
        mov ebp, esp
        sub esp, 8
        mov eax, __NR_%1
        mov ebx, %3
        int 0x80
        mov dword [ebp-4], eax
        cmp eax, 0
        jge .syscall_return
        mov eax, -1
    .syscall_return:
        mov esp, ebp
        pop ebp
        ret
%endmacro

%macro _syscall1 5
    global %1
    %1:
        push ebp
        mov ebp, esp
        sub esp, 12
        mov eax, __NR_%1
        mov ebx, %3
        mov ecx, %5
        int 0x80
        mov dword [ebp-4], eax
        cmp eax, 0
        jge .syscall_return
        mov eax, -1
    .syscall_return:
        mov esp, ebp
        pop ebp
        ret
%endmacro

%macro _syscall2 7
    global %1
    %1:
        push ebp
        mov ebp, esp
        sub esp, 16
        mov eax, __NR_%1
        mov ebx, %3
        mov ecx, %5
        mov edx, %7
        int 0x80
        mov dword [ebp-4], eax
        cmp eax, 0
        jge .syscall_return
        mov eax, -1
    .syscall_return:
        mov esp, ebp
        pop ebp
        ret
%endmacro

%macro _syscall3 9
    global %1
    %1:
        push ebp
        mov ebp, esp
        sub esp, 20
        mov eax, __NR_%1
        mov ebx, %3
        mov ecx, %5
        mov edx, %7
        mov esi, %9
        int 0x80
        mov dword [ebp-4], eax
        cmp eax, 0
        jge .syscall_return
        mov eax, -1
    .syscall_return:
        mov esp, ebp
        pop ebp
        ret
%endmacro

%macro _syscall4 11
    global %1
    %1:
        push ebp
        mov ebp, esp
        sub esp, 24
        mov eax, __NR_%1
        mov ebx, %3
        mov ecx, %5
        mov edx, %7
        mov esi, %9
        mov edi, %11
        int 0x80
        mov dword [ebp-4], eax
        cmp eax, 0
        jge .syscall_return
        mov eax, -1
    .syscall_return:
        mov esp, ebp
        pop ebp
        ret
%endmacro

extern syscall_handler
global isr80_handler
isr80_handler:
    cli
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call syscall_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popa
    sti
    iret