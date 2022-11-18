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
	align 0x1000

; Multiboot Section
extern __lhk_multiboot
align 4
call __lhk_multiboot

section .bss
	align 32
stack:
	resb STACK_SIZE

section .text
	global _start:function (_start.end - _start)
	extern kmain

_start:
	xor ebp, ebp
	mov esp, stack + STACK_SIZE 
	push ebx
	push eax
	
	cli
	call kmain
	pop eax
	cmp eax, 1
	jmp display_error_msg
.hang:
	hlt
	jmp .hang
.end: