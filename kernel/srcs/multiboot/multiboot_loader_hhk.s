bits 32
section .text

extern init_multiboot_kernel

extern BOOTLOADER_MAGIC

; HHK: Higher Half Kernel
global hhk_loader:function
hhk_loader:
    align 4
    mov ebx, BOOTLOADER_MAGIC
    push ebx
    call init_multiboot_kernel
    pop ebx
    cmp eax, 0
    jnz .error
    jmp .ret

.error:
    mov eax, -1
    ret

.ret:
    mov eax, 0
    ret