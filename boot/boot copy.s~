%define MBALIGN 1 << 0
%define MEMINFO 1 << 1
%define FLAGS (MBALIGN | MEMINFO)
%define MAGIC 0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

%define KERNEL_STACK_SIZE 4096

%define KERNEL_VIRTUAL_BASE 0xC0000000
%define PDE_INDEX (KERNEL_VIRTUAL_BASE >> 22) ; Index in page Directory
%define PSE_BIT 0x00000010
%define PG_BIT 0x80000000

bits 32

global _multiboot
extern _code_section
extern _bss_section
extern _kernel_end
extern _kernel_start

section .multiboot
_multiboot:
	align 4

	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
	align 16

	stack_bottom:
	resb KERNEL_STACK_SIZE ; Kernel STACK = 16 KiB
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

	lea ebx, [_start]
    jmp ebx

	; jmp _start
	.end:

_start:
	; mov dword [_page_directory], 0
	; invlpg[0]
	; mov esp, stack_top
	; extern kmain
	; push ebx
	; call kmain
	; cli

	mov dword[_page_directory], 0
	invlpg[0]

	mov esp, stack_top
	extern kmain
	push ebx

	call kmain
	cli

.hang:
	hlt
	jmp .hang
.end: