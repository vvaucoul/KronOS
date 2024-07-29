;*******************************************************************************
;*                                   DEFINES                                   *
;*******************************************************************************

STACK_SIZE equ 0x4000
KERNEL_STACK_MARKER equ 0x4B52304E ; "KRONOST" (KRONOS STACK) - Marker for stack initialization

;*******************************************************************************
;*                              STACK INITIALIZER                              *
;*******************************************************************************

section .bss
    align 32
stack:
    resb STACK_SIZE

section .text
    global initialize_stack
    extern stack

initialize_stack:
    ; Initialize stack with marker
    mov ecx, STACK_SIZE / 4          ; Number of 32-bit words in stack
    mov edi, stack                   ; Destination address
    mov eax, KERNEL_STACK_MARKER     ; Marker value
    rep stosd                        ; Initialize stack with marker
    ret