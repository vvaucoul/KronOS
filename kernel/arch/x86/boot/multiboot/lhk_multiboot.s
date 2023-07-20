;*******************************************************************************
;*                              MULTIBOOT VALUES                               *
;*******************************************************************************

MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
MBOOT_VIDEO equ 1 << 2
FLAGS equ (MBALIGN | MEMINFO | MBOOT_VIDEO)
MAGIC equ 0x1BADB002
BOOTLOADER_MAGIC equ 0x2BADB002
CHECKSUM equ -(MAGIC + FLAGS)

global BOOTLOADER_MAGIC

[GLOBAL __lhk_multiboot]
[EXTERN __kernel_text_section_start]
[EXTERN __kernel_bss_section_start]
[EXTERN __kernel_section_end]
[EXTERN _start]

;*******************************************************************************
;*                           MULTIBOOT INIT FUNCTION                           *
;*******************************************************************************

bits 32

__lhk_multiboot:
section .multiboot.data
	align 4
    MultiBootHeader:
	dd MAGIC		; Magic number
	dd FLAGS		; Flags
	dd CHECKSUM		; Checksum

	dd __lhk_multiboot						; Descriptor address
	dd __kernel_text_section_start			; Load start address (.txt kernel section)
	dd __kernel_bss_section_start			; BSS start address (end .data section)
	dd __kernel_section_end					; Load end address
	dd _start								; Entry address (Kernel entrypoint initial EIP)

	dd 1			; Mode type
	dd 0			; Width
	dd 0			; Height
	dd 32			; Depth

	ret