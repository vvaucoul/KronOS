;*******************************************************************************
;*                                   DEFINES                                   *
;*******************************************************************************

STACK_SIZE equ 0x4000

;*******************************************************************************
;*                                  MULTIBOOT                                  *
;*******************************************************************************

global __call_kmain
extern BOOTLOADER_MAGIC
extern display_error_msg

section .data
	align 4096

section .initial_stack, nobits
    align 4

section .bss
	align 16
	stack_bottom:
	resb STACK_SIZE
	stack_top:

section .text
	global _start:function (_start.end - _start)
	extern kmain

_start:
	mov esp, stack_top
	mov eax, BOOTLOADER_MAGIC
	push ebx
	push eax
	call kmain
	cli
	call display_error_msg
.hang:
	hlt
	jmp .hang
.end: