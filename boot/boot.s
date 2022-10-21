;*******************************************************************************
;*                                   DEFINES                                   *
;*******************************************************************************

STACK_SIZE equ 0x19998 ; 1MB stack

global __call_kmain
extern display_error_msg
extern hhk_loader

;*******************************************************************************
;*                        HIGHER HALF KERNEL CONSTANTS                         *
;*******************************************************************************

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PHYSICAL_BASE equ 0x00100000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE) >> 22
PSE_BIT equ 0x00000010 ; Page Size Extension bit
PG_BIT equ 0x80000000 ; Page Global bit

;*******************************************************************************
;*                                  SECTIONS                                   *
;*******************************************************************************

; Data Section
section .data
align 0x1000
; global BootPageDirectory
BootPageDirectory:
	dd 0x00000083 ; 10000011 - dd: 4bytes constant
    times (KERNEL_PAGE_NUMBER - 1) dd 0
    dd 0x00000083 ; 10000011
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

; Multiboot Section
extern __hhk_multiboot
align 4
call __hhk_multiboot

; Kernel Entry
section .text
align 4
global _lower_half_kernel

extern kmain

lower_half_kernel:
global lower_half_kernel

_lower_half_kernel:
	cli
	; Update Page Directory
	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
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
	mov dword[BootPageDirectory], 0
	invlpg[0]

	mov esp, stack + STACK_SIZE 

	push ebx
	push eax

	cli
	call kmain
	pop eax
	cmp eax, 1
	call display_error_msg
	hlt
.hang:
	hlt
	jmp .hang

; Stack Section
section .bss
	align 32
stack:
	resb STACK_SIZE
