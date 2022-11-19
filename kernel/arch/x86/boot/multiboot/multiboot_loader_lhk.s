bits 32
section .text

extern init_multiboot_kernel

extern BOOTLOADER_MAGIC

; LHK: Lower Half Kernel
global lhk_loader:function
lhk_loader:
    align 4 

    ; mov ecx, ebx
    mov ebx, BOOTLOADER_MAGIC
    ; mov ebx, 1 ; test

    push ebx
    ; push ecx

    call init_multiboot_kernel

    pop ebx
    ; pop ecx

    cmp eax, 0
    jnz .error
    jmp .ret

.error:
    mov eax, -1
    ret

.ret:
    mov eax, 0
    ret