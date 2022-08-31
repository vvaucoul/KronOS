section .text
global load_page_directory

load_page_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    mov cr3, eax
    mov esp, ebp
    pop ebp
    ret

    ; mov cr3, eax
    ; mov ebx, cr4        ; read current cr4
    ; or  ebx, 0x00000010 ; set PSE
    ; mov cr4, ebx        ; update cr4
    ; mov ebx, cr0        ; read current cr0
    ; or  ebx, 0x80000000 ; set PG
    ; mov cr0, ebx        ; update cr0