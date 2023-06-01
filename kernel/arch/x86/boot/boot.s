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

;*******************************************************************************

;*******************************************************************************
;*                                   DEFINES                                   *
;*******************************************************************************

STACK_SIZE equ 104856 ; 1024 * 1024 (1MB)

global __call_kmain
extern display_error_msg

;*******************************************************************************
;*                        HIGHER HALF KERNEL CONSTANTS                         *
;*******************************************************************************

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PHYSICAL_BASE equ 0x00100000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

PSE_BIT equ 0x00000010 ; Page Size Extension bit
PG_BIT equ 0x80000000 ; Page Global bit

;*******************************************************************************
;*                                  SECTIONS                                   *
;*******************************************************************************

bits 32

; Multiboot Section
section .multiboot
align 4
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

; Data Section
section .data
align 0x1000

global BOOT_PAGE_DIRECTORY
BOOT_PAGE_DIRECTORY:
	dd 0x00000083 ; 10000011 - dd: 4bytes constant
    times (KERNEL_PAGE_NUMBER - 1) dd 0
    dd 0x00000083 ; 10000011
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

; Stack Section
section .initial_stack, nobits
	align 4
stack_bottom:
	resb STACK_SIZE
stack_top:

; Kernel Entry
section .text
global _start
_start equ (_kernel_entry - KERNEL_VIRTUAL_BASE)

global _kernel_entry
_kernel_entry:
	; Update Page Directory
	mov ecx, (BOOT_PAGE_DIRECTORY - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx

	; Enable 4MB Pages
	mov ecx, cr4
	mov ecx, PSE_BIT
	mov cr4, ecx

	; Set PG bit in CR0 (enable paging)
	mov ecx, cr0
	or ecx, PG_BIT
	mov cr0, ecx

	; Jump to higher half kernel (jump virtual memory)
	lea ecx, [higher_half_kernel]
	jmp ecx

higher_half_kernel:
	; Unmap the first 4MB of memory
	mov dword[BOOT_PAGE_DIRECTORY], 0
	invlpg[0]

	mov esp, stack_top

	extern kmain
	
	push esp ; stack
	push ebx ; multiboot info
	push eax ; magic number

	call kmain
	pop eax
	cmp eax, 1
	call display_error_msg
	hlt

.hang:
	hlt
	jmp .hang