%define MBALIGN 1 << 0
%define MEMINFO 1 << 1
%define FLAGS (MBALIGN | MEMINFO)
%define MAGIC 0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

%define KERNEL_STACK_SIZE 16384
%define KERNEL_PAGE_SIZE 4096

%define KERNEL_VIRTUAL_BASE 0xC0000000
%define KERNEL_PAGE_NUMBER  (KERNEL_VIRTUAL_BASE >> 22) ; Index in page Directory
%define PSE_BIT 0x00000010
%define PG_BIT 0x80000000

bits 32

global _multiboot
extern _code_section
extern _bss_section
extern _kernel_end
extern _kernel_start

; Declare a multiboot header that marks the program as a kernel.
section .multiboot.data
_multiboot:
	align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .data
align 0x100
global TEMP_PAGE_DIRECTORY
TEMP_PAGE_DIRECTORY:
	dd 0x00000083
    times(KERNEL_PAGE_NUMBER  - 1) dd 0
    dd 0x00000083
    times(1024 - KERNEL_PAGE_NUMBER  - 1) dd 0 

; Allocate the initial stack.
section .bootstrap_stack, nobits
	align 4
	stack_bottom:
	resb KERNEL_STACK_SIZE ; Kernel STACK = 16 KiB
	stack_top:

;section .bss
	;align 4096
	;global _page_directory:function
	;_page_directory:
	;resb KERNEL_PAGE_SIZE

	;align 4096
	;_page_table:
	;resb KERNEL_PAGE_SIZE
	; Further page tables may be required if the kernel grows beyond 3 MiB

section .text
	global _kernel_entry:function (_kernel_entry.end - _kernel_entry)
	global _start:function (_start.end - _start)
	extern _kernel_start
	extern _kernel_end
	low_kernel_entry equ (_kernel_entry - KERNEL_VIRTUAL_BASE)

_kernel_entry:
	mov ecx, (TEMP_PAGE_DIRECTORY - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

	; Enable 4mb pages
;    mov ecx, cr4;
;     or ecx, PSE_BIT
;    mov cr4, ecx

    ; Set PG bit, enable paging
    mov ecx, cr0
	or ecx, PG_BIT
    mov cr0, ecx

	lea ecx, [_higher_half_kernel]
    jmp ecx
	_kernel_entry.end:

	;mov edi, _page_table - 0xC0000000
	;mov esi, 0
	;mov ecx, 1023

; _page_directory_start:
; 	mov dword [edi], esi
; 	add edi, 4
; 	loop _page_directory_start

; 	mov edi, _page_table
; 	sub edi, 0xC0000000
; 	mov esi, 0
; 	mov ecx, 1024
; _kernel_entry.end:

; _virtual_kernel_map_start:
; 	mov edx, esi
; 	or edx, 0x03
; 	mov dword [edi], edx
; 	add esi, 4096
; 	add edi, 4
; 	loop _virtual_kernel_map_start

; 	mov ecx, _page_directory
; 	sub ecx, 0xC0000000
; 	mov dword [ecx], _page_table - 0xC0000000 + 0x03
; 	mov dword [ecx + 768 * 4], _page_table - 0xC0000000 + 0x03

; 	mov cr3, ecx

; 	mov ecx, cr0
; 	or ecx, 0x80000001
; 	mov cr0, ecx

; 	lea ecx, [_virtual_kernel_map_end]
; 	jmp ecx

; section .text
; _virtual_kernel_map_end:
; 	mov dword [_page_directory], 0

; 	mov ecx, cr3
; 	mov cr3, ecx

; 	jmp _start

_higher_half_kernel:
	mov dword[TEMP_PAGE_DIRECTORY], 0
    invlpg[0]

	mov esp, stack_top

	; mov edx, _page_directory
	push ebx
	; push edx

	extern kmain
	call kmain

	; cli

.hang:
	hlt
	jmp .hang
.end: