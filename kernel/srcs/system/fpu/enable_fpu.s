section .text
global __enable_fpu:function

__enable_fpu:
    ret
    ; mov cr4, [0]
    ; mov cr4, 0b001000000000
    ; fldcw [0x37F]
    ; fldcw [0x37E]
    ; fldcw [0x37A]
