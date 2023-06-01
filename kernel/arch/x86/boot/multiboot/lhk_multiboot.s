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

;*******************************************************************************
;*                           MULTIBOOT INIT FUNCTION                           *
;*******************************************************************************

bits 32
global __lhk_multiboot

__lhk_multiboot:
section .multiboot.data
	align 4
    MultiBootHeader:
	dd MAGIC		; Magic number
	dd FLAGS		; Flags
	dd CHECKSUM		; Checksum

	dd 0			; Header address
	dd 0			; Load address
	dd 0			; Load end address
	dd 0			; BSS end address
	dd 0			; Entry address

	dd 1			; Mode type
	dd 0			; Width
	dd 0			; Height
	dd 32			; Depth

	ret