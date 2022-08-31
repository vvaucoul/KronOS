%define MBALIGN 1 << 0
%define MEMINFO 1 << 1
%define FLAGS (MBALIGN | MEMINFO)
%define MAGIC 0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

%define KERNEL_STACK_SIZE 16384

; Loader Values(Linker.ld)
extern __kernel_virtual_memory_start
extern __kernel_virtual_memory_end
extern __kernel_physical_memory_start
extern __kernel_physical_memory_end

bits 32
section .multiboot
align 16
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
	align 16
	stack_bottom:
	resb KERNEL_STACK_SIZE
	stack_top:

section .text
	global _start:function (_start.end - _start)
	extern kmain

_start:
	mov esp, stack_top
	
	push ebx
	push __kernel_virtual_memory_start
	push __kernel_virtual_memory_end
	push __kernel_physical_memory_start
	push __kernel_physical_memory_end
	
	call kmain
	cli
.hang:
	hlt
	jmp .hang	
.end: