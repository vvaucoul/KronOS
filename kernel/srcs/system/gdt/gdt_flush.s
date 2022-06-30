global gdt_flush     ; Allows the C code to link to this
gdt_flush:
    mov eax , [esp+4]
    lgdt [eax]        ; Load the GDT with our 'gp' which is a special pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ax, 0x18         ; 0x18 is the offset in the GDT to our kernel stack
    mov ss, ax
    jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               ; Returns back to the C code!