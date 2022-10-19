;*******************************************************************************
;*                                   DEFINES                                   *
;*******************************************************************************

STACK_SIZE equ 0x19998 ; 1MB stack

global __call_kmain
extern BOOTLOADER_MAGIC
extern display_error_msg
extern hhk_loader

;*******************************************************************************
;*                        HIGHER HALF KERNEL CONSTANTS                         *
;*******************************************************************************

KERNEL_VIRTUAL_MEMORY equ 0xC0000000
PDE_INDEX equ KERNEL_VIRTUAL_MEMORY >> 22
PSE_BIT equ 0x00000010
PG_BIT equ 0x80000000

;*******************************************************************************
;*                                  SECTIONS                                   *
;*******************************************************************************

; Multiboot Section
section .multiboot
	extern __init_multiboot
	align 4
	call __init_multiboot

; Data Section
section .data
align 4096
global __kernel_tmp_page_directory
__kernel_tmp_page_directory:
	dd 0x00000083 ; dd 4-byte constants
	times(PDE_INDEX - 1) dd 0
	dd 0x00000083
	times(1024 - PDE_INDEX - 1) dd 0

; Stack Section
section .bss, nobits
	align 4
	resb STACK_SIZE
	stack_top:

; Kernel Entry
section .text
global _higher_half_kernel
global low_kernel_entry

extern kmain

low_kernel_entry:
	jmp _kernel_entry
_kernel_entry:
	; Update Page Directory
	mov ecx, (__kernel_tmp_page_directory - KERNEL_VIRTUAL_MEMORY)
	mov cr3, ecx

	; Enable 4MB Pages
	mov eax, cr4
	mov ecx, PSE_BIT
	mov cr4, ecx

	; Set PG bit in CR0 (enable paging)
	mov ecx, cr0
	or ecx, PG_BIT
	mov cr0, ecx

	; Jump to higher half kernel (jump virtual memory)
	lea ecx, [_higher_half_kernel]
	jmp ecx

_higher_half_kernel:
	; Unmap the first 4MB of memory
	mov dword[__kernel_tmp_page_directory], 0
	invlpg[0]

	mov esp, stack_top
	push ebx
	call kmain
	cli
	call display_error_msg
.hang:
	hlt
	jmp .hang