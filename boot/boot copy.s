;*******************************************************************************
;*                          MULTIBOOT HEADER DEFINES                           *
;*******************************************************************************

%define MBOOT_MBALIGN 1 << 0
%define MBOOT_MEM_INFO 1 << 1
%define MBOOT_HEADER_FLAGS (MBOOT_MBALIGN | MBOOT_MEM_INFO)
%define MBOOT_HEADER_MAGIC 0x1BADB002
%define MBOOT_CHECKSUM -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

;*******************************************************************************
;*                            KERNEL DEFINES VALUES                            *
;*******************************************************************************

%define KERNEL_STACK_SIZE 0x4000
%define KERNEL_PAGE_SIZE 0x1000

%define KERNEL_VIRTUAL_BASE 0xC0000000
%define KERNEL_PAGE_NUMBER  (KERNEL_VIRTUAL_BASE >> 22) ; Index in page Directory

%define PSE_BIT 0x00000010
%define PG_BIT 0x80000000

;*******************************************************************************
;*                      PMM / CONTIGUOUS PHYSICAL MEMORY                       *
;*******************************************************************************

%define PMM_VIRTUAL_BASE 0xB0000000
%define PMM_PAGE_NUMBER (PMM_VIRTUAL_BASE >> 22) ; Index in page Directory

global _kernel_entry
extern kmain

section .data
align 0x1000
KernelPageDirectory:
	dd 0x00000083
    times (PMM_PAGE_NUMBER - 1) dd 0
    dd 0xB0000083 
    times (KERNEL_PAGE_NUMBER - PMM_PAGE_NUMBER - 1) dd 0 
    dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0


; Declare a multiboot header that marks the program as a kernel.
section .text
align 4
_multiboot:
	dd MBOOT_HEADER_MAGIC
	dd MBOOT_HEADER_FLAGS
	dd MBOOT_CHECKSUM

_start equ (_kernel_entry - 0xC0000000)
global _start

_kernel_entry:
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

	lea ecx, [_higher_half_kernel]
    jmp ecx

_higher_half_kernel:
	mov dword[KernelPageDirectory], 0
    invlpg[0]

	mov esp, stack + KERNEL_STACK_SIZE

	push ebx
	mov ebp, 0

	call kmain
	hlt

; Allocate the initial stack.
section .bss
align 32
stack:
	stack_bottom:
	resb KERNEL_STACK_SIZE ; Kernel STACK = 16 KiB
	stack_top:
