%define MBALIGN 1 << 0
%define MEMINFO 1 << 1
%define FLAGS (MBALIGN | MEMINFO)
%define MAGIC 0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

%define KERNEL_STACK_SIZE 16384
%define KERNEL_PAGE_SIZE 4096

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

section .multiboot.data
_multiboot:
	align 4

	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
	align 4096
	global _page_directory:function
	_page_directory:
	resb KERNEL_PAGE_SIZE

	align 4096
	_page_table:
	resb KERNEL_PAGE_SIZE

	align 4096
	stack_bottom:
	resb KERNEL_STACK_SIZE ; Kernel STACK = 16 KiB
	align 4096
	stack_top:

section .multiboot.text
	global _kernel_entry:function (_kernel_entry.end - _kernel_entry)
	global _start:function (_start.end - _start)
	extern _kernel_start
	extern _kernel_end

_kernel_entry:
	mov edi, _page_directory
	mov esi, 0
	mov ecx, 1024

_page_directory_start:
	mov dword [edi], esi
	add edi, 4
	loop _page_directory_start

	mov edi, _page_table
	sub edi, 0xC0000000
	mov esi, 0
	mov ecx, 1024
_kernel_entry.end:

_virtual_kernel_map_start:
	mov edx, esi
	or edx, 0x03
	mov dword [edi], edx
	add esi, 4096
	add edi, 4
	loop _virtual_kernel_map_start

	mov ecx, _page_directory
	sub ecx, 0xC0000000
	mov dword [ecx], _page_table - 0xC0000000 + 0x03
	mov dword [ecx + 768 * 4], _page_table - 0xC0000000 + 0x03

	mov cr3, ecx

	mov ecx, cr0
	or ecx, 0x80000001
	mov cr0, ecx

	lea ecx, [_virtual_kernel_map_end]
	jmp ecx

section .text
_virtual_kernel_map_end:
	mov dword [_page_directory], 0

	mov ecx, cr3
	mov cr3, ecx

	jmp _start

_start:
	mov esp, stack_top
	xor ebp, ebp

	mov edx, _page_directory
	push edx
	push ebx
	push eax

	extern kmain
	call kmain

	cli

.hang:
	hlt
	jmp .hang
.end: