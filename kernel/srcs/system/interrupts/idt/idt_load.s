; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
global idt_load

idt_load:
    cli ; Disable interrupts
    mov eax, [esp + 4]
    lidt [eax] ; Load IDT
    sti ; Enable interrupts
    ret