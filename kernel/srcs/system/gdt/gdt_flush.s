%define DATA_SEGMENT_OFFSET 0x10
%define CODE_SEGMENT_OFFSET 0x08

bits 32

section .text
global gdt_flush							; Allows the C code to link to this
gdt_flush:
	mov eax, [esp + 4]						; Load the gdt parameter
	lgdt [eax]         			    		; Load the GDT with our 'gp' which is a special pointer | LGDT -> Load GDT
	mov ax, DATA_SEGMENT_OFFSET     		; 0x10 is the offset in the GDT to our data segment
	
	mov ds, ax             		   			; Data segment
	mov es, ax             		   			; Extra segment
	mov fs, ax             		   			; Pointer to more extra data
	mov gs, ax             		   			; Pointer to still more extra data

	mov ax, 0x18           		   			; 0x18 is the offset in the GDT to our kernel stack
	mov ss, ax             		   			; Stack Segment
	jmp CODE_SEGMENT_OFFSET:.code_selector 	; 0x08 is the offset to our code segment: Far jump!
.code_selector:
	ret                    		   			; Returns back to the C code!