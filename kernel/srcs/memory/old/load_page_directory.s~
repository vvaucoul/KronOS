section .text
global load_page_directory

load_page_directory:
    ; push ebp
    ; mov ebp, esp
    ; mov eax, [ebp + 8]
    ; mov cr3, eax
    ; mov esp, ebp
    ; pop ebp
    ; ret

    mov eax, [esp + 4]
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret