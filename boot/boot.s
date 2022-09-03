;*******************************************************************************
;*                          MULTIBOOT HEADER DEFINES                           *
;*******************************************************************************

MBOOT_MBALIGN equ 1 << 0
MBOOT_MEM_INFO equ 1 << 1
MBOOT_HEADER_FLAGS equ (MBOOT_MBALIGN | MBOOT_MEM_INFO)
MBOOT_HEADER_MAGIC equ 0x1BADB002
MBOOT_CHECKSUM equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

;*******************************************************************************
;*                            KERNEL DEFINES VALUES                            *
;*******************************************************************************

KERNEL_STACK_SIZE equ 0x4000
KERNEL_PAGE_SIZE equ 0x1000

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22) ; Index in page Directory

PSE_BIT equ 0x00000010
PG_BIT equ 0x80000000

;*******************************************************************************
;*                      PMM / CONTIGUOUS PHYSICAL MEMORY                       *
;*******************************************************************************

PMM_VIRTUAL_BASE equ 0xB0000000
PMM_PAGE_NUMBER equ (PMM_VIRTUAL_BASE >> 22) ; Index in page Directory

;*******************************************************************************
;*                                CODE SECTIONS                                *
;*******************************************************************************

; Multiboot Section | For Grub
section .multiboot.data
align 4
; Declare a multiboot header that marks the program as a kernel.
multiboot:
	dd MBOOT_HEADER_MAGIC
	dd MBOOT_HEADER_FLAGS
	dd MBOOT_CHECKSUM

; Allocate the initial stack.
section .bss
align 16
	stack_bottom:
	resb KERNEL_STACK_SIZE ; Kernel STACK = 16 KiB
	stack_top:

; Data Section
section .data
align 0x1000
KernelPageDirectory:
	dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0
   ; dd 0xB0000083 
    ;times (KERNEL_PAGE_NUMBER - PMM_PAGE_NUMBER - 1) dd 0 
    dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

; Text Section
section .text
	global kernel_entry
	global low_kernel_entry

low_kernel_entry equ (kernel_entry - KERNEL_VIRTUAL_BASE)

kernel_entry:
	mov ecx, (KernelPageDirectory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

	; Enable 4mb pages
   	mov ecx, cr4
    or ecx, PSE_BIT
   	mov cr4, ecx

    ; Set PG bit, enable paging
    mov ecx, cr0
	or ecx, PG_BIT
    mov cr0, ecx

	lea ecx, [higher_half_kernel]
    jmp ecx

higher_half_kernel:
	mov dword[KernelPageDirectory], 0
    invlpg[0]

	mov esp, stack_top
	push ebx

	extern kmain
	call kmain
	cli
.hang:
	hlt
	jmp .hang	
.end: