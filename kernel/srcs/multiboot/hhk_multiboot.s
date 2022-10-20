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
global __hhk_multiboot

__hhk_multiboot:
section .text
	align 4
    MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

	dd 0
	dd 0
	dd 0
	dd 0
	dd 0

	dd 1
	dd 800
	dd 400
	dd 32