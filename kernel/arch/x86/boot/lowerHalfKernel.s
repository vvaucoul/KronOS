;*******************************************************************************
;*                                   DEFINES                                   *
;*******************************************************************************

STACK_SIZE equ 0x4000
KERNEL_STACK_MARKER equ 0x4B52304E

;*******************************************************************************
;*                                  MULTIBOOT                                  *
;*******************************************************************************

global __call_kmain
extern BOOTLOADER_MAGIC
extern display_error_msg
extern initialize_stack

section .data
    align 0x1000

; Multiboot Section
align 4
extern __lhk_multiboot

section .bss
    align 32
extern stack

section .text
    global _start:function (_start.end - _start)
    extern kmain

_start:
    xor ebp, ebp

	; Initialize_stack use EAX, so we need to save it
	push eax
    ; Call stack initializer
    call initialize_stack
    pop eax

    mov esp, stack + STACK_SIZE
    push esp                         ; stack
    push ebx                         ; Multiboot Info
    push eax                         ; Magic Number
    
    cli
    call kmain
    test eax, eax
    jz .hang
    cmp eax, 1
    jz .error

.error:
    call display_error_msg
    hlt
    jmp .error

.hang:
    hlt
    jmp .hang

.end:
