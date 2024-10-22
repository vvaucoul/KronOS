bits 32
global switch_page_directory
switch_page_directory:
    ; Set the page directory's physical address
    mov eax, [esp + 4]  ; Get the first argument (dir->physicalAddr)
    mov cr3, eax        ; Move it to CR3
    ; Flush the TLB
    mov eax, cr3
    mov cr3, eax
    ret
