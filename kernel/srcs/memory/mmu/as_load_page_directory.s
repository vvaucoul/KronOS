bits 32
global load_page_directory
load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret