; ------------------------------------------------------------------------------------------------------------------
; BIOS INT 0x12 - Get Memory Size (Conventional Memory)
; BIOS INT 0x15 Function 0xE881 - Get Memory Size For > 64 MB Configuations (32 Bit)
; BIOS INT 0x15 Function 0xE801 - Get Memory Size For > 64 MB Configuations
;
; Return:
; CF = Clear if successful
; EAX = Extended memory between 1MB and 16MB in KB
; EBX = Extended memory above 16MB, in 64KB blocks
; ECX = Configured memory 1MB to 16MB in KB
; EDX = Configured memory above 16MB in 64JB blocks
; This interrupt is exactally like INT 0x15 Function 0xE801 except it uses the extended registers (EAX/EBX/ECX/EDX).
; ------------------------------------------------------------------------------------------------------------------

;---------------------------------------------
;	Get memory size for >64M configuations
;	ret\ ax=KB between 1MB and 16MB
;	ret\ bx=number of 64K blocks above 16MB
;	ret\ bx=0 and ax= -1 on error
;---------------------------------------------

BIOS_MEMORY_INSTRUCTION equ 0x15
BIOS_MEMORY_FUNCTION equ 0xE881

ERROR_UNSUPPORTED_FUNCTION equ 0x86
ERROR_INVALID_COMMAND equ 0x80

section .text
global get_memory_size:function

get_memory_size:
    push ecx
    push edx
    xor ecx, ecx
    xor edx, edx
    mov ax, BIOS_MEMORY_FUNCTION
    int BIOS_MEMORY_INSTRUCTION
    jc .error
    cmp ah, ERROR_UNSUPPORTED_FUNCTION
    je .error
    cmp	ah, ERROR_INVALID_COMMAND
	je	.error
    jcxz .use_ax
    mov ax, cx
    mov bx, dx

.use_ax:
    pop edx
    pop ecx
    ret

.error:
    mov ax, -1
    mov bx, 0
    pop edx
    pop ecx
    ret