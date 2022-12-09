; constants for multiboot header
MEMINFO      equ  1<<0
BOOTDEVICE   equ  1<<1
CMDLINE      equ  1<<2
FLAGS        equ  MEMINFO | BOOTDEVICE | CMDLINE 
MAGIC_HEADER equ  0x1BADB002
CHECKSUM     equ -(MAGIC_HEADER + FLAGS)

BOOTLOADER_MAGIC  equ  0x2BADB002
STACK_SIZE equ 0x4000


extern __kernel_section_end
extern __kernel_bss_section_end
extern __multiboot_start

; set multiboot section
section .multiboot
    align 4
_multiboot_header:
    dd MAGIC_HEADER
    dd FLAGS
    dd CHECKSUM

%ifndef __ELF__
    dd _multiboot_header
	dd _start
	dd __kernel_section_end
	dd __kernel_bss_section_end
	dd __multiboot_start
%else ; /* __ELF__ */
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
%endif ; /* __ELF__ */

	dd 1 ; 0 for VESA VBE - 1 for VGA
	dd 640
	dd 480
	dd 16

section .data
    align 4096

; initial stack
section .initial_stack, nobits
    align 4

stack_bottom:
    ; 1 MB of uninitialized data for stack
    resb STACK_SIZE
stack_top:

; kernel entry, main text section
section .text
    global _start
    global MAGIC_HEADER
    global BOOTLOADER_MAGIC


; define _start, aligned by linker.ld script
_start:
    mov esp, stack_top
    
    ; Reset EFLAGS
    push dword 0
    popf

    extern kmain
    mov eax, BOOTLOADER_MAGIC
    push ebx
    push eax
    call kmain
loop:
    jmp loop

