; bits 32

; extern test_user_function

; ; global switch_user_mode
; ; switch_user_mode:
; ;     mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
; ; 	mov ds, ax
; ; 	mov es, ax 
; ; 	mov fs, ax 
; ; 	mov gs, ax ; SS is handled by iret
    
; ; 	; set up the stack frame iret expects
; ; 	mov eax, esp
; ; 	push (4 * 8) | 3 ; data selector
; ; 	push eax ; current esp
; ; 	pushf ; eflags
; ; 	push (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
; ; 	push test_user_function ; instruction address to return to
; ; 	iret

; %define USER_CODE_SEGMENT 0x23

; section .data
; error_message db "Error: switch_to_user_mode called with NULL stack pointer", 0

; extern printk

; section .text
; global switch_to_user_mode

; switch_to_user_mode:
; 	; Load the user code segment
; 	mov ax, USER_CODE_SEGMENT
; 	mov ds, ax
; 	mov es, ax
; 	mov fs, ax
; 	mov gs, ax
; 	push ax

; 	; Load the user stack pointer
; 	mov eax, [esp + 4]
; 	cmp eax, 0
; 	je .error
; 	mov esp, eax

; 	; Set the mode bit
; 	pushf
; 	pop eax
; 	or eax, 0x200
; 	push eax
; 	popf
; 	ret


; .error:
; 	mov eax, error_message
; 	call printk
; 	jmp $

