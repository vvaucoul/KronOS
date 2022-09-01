%define MBALIGN 1 << 0
%define MEMINFO 1 << 1
%define FLAGS (MBALIGN | MEMINFO)
%define MAGIC 0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

bits 32

global _multiboot
extern _code_section
extern _bss_section
extern _kernel_end
extern _kernel_start

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

	; dd _multiboot
	; dd _code_section
	; dd _bss_section
	; dd _kernel_end
	; dd _kernel_start

section .bss
	align 16

	stack_bottom:
	resb 16384
	stack_top:

section .data
	align 0x100
	global _page_directory:function

	_page_directory:
	dd 0x00000083
	times(PDE_INDEX - 1) dd 0
	dd 0x00000083
	times(1024 - PDE_INDEX - 1) dd 0

section .text
	global _start:function (_start.end - _start)
	global _kernel_entry:function (_kernel_entry.end - _kernel_entry)
	extern kmain

_kernel_entry:
	mov ecx, (_page_directory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx

	; mov ecx, cr4
	; or ecx, PSE_BIT
	; mov cr4, ecx
	
	; mov ecx, cr0
    ; or ecx, PG_BIT
    ; mov cr0, ecx

	lea ecx, [_start]
    jmp ecx

	; jmp _start
	.end:

_start:

	mov dword [_page_directory], 0
	invlpg[0]

	mov esp, stack_top
	call kmain
	cli

	; mov dword[_page_directory], 0
	; invlpg[0]

	; mov esp, stack_top
	; extern kmain
	; push ebx

	; call kmain
	; cli
.hang:
	hlt
	jmp .hang
.end: