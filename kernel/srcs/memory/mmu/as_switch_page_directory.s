bits 32
global switch_page_directory
switch_page_directory:
    ; Set the page directory's physical address
    mov eax, [esp + 4]  ; Get the first argument (dir->physicalAddr)
    mov cr3, eax        ; Move it to CR3

    ; Read CR0 register
    mov eax, cr0

    ; Enable paging by setting the paging bit (bit 31)
    or eax, 0x80000000

    ; Write back to CR0
    mov cr0, eax

    ret