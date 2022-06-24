%define MBALIGN 1 << 0
%define MEMINFO 1 << 1
%define FLAGS (MBALIGN | MEMINFO)
%define MAGIC 0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

bits 32
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
	align 16
	stack_bottom:
	resb 16384 ; 16 KiB
	stack_top:

section .text
	global _start:function (_start.end - _start)
	extern kmain

_start:
	mov esp, stack_top
	call kmain
	cli
.hang:
	hlt
	jmp .hang
.end: