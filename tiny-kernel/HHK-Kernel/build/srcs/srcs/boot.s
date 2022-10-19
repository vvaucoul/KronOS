global _lower_half_kernel
extern _main

MEMALIGN equ 1 << 0
MEMINFO equ 1 << 1
MEMVIDEO equ 1 << 2
FLAGS equ MEMALIGN | MEMINFO | MEMVIDEO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PHYSICAL_BASE equ 0x00100000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE) >> 22

section .data
align 0x1000
BootPageDirectory:
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

section .text
align 4
MultiBootHeader:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

STACK_SIZE equ 0x4000

lower_half_kernel equ (_lower_half_kernel - 0xC0000000)
global lower_half_kernel

_lower_half_kernel:
    mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    lea ecx, [higher_half_kernel]
    jmp ecx

higher_half_kernel:
    mov dword [BootPageDirectory], 0
    invlpg[0]

    mov esp, stack + STACK_SIZE
    
    push eax
    push ebx

    extern kmain
    call kmain
    hlt

section .bsss
    align 32
stack:
    resb STACK_SIZE