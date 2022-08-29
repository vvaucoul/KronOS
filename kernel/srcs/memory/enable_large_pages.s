%define LARGE_PAGE_ENABLE_VALUE 0x00000010

section .text
global enable_large_pages

enable_large_pages:
    mov eax, cr4
    or eax, LARGE_PAGE_ENABLE_VALUE
    mov cr4, eax