OUTPUT_FORMAT(elf32-i386)
ENTRY(lower_half_kernel)
 
SECTIONS {
    . = 0x100000;

    .multiboot.text : 
	{
		*(.multiboot.text)
	}
	.multiboot.data : 
	{
		*(.multiboot.data)
	}
    .multiboot.bss :
    {
        *(.multiboot.bss)
    }

    . += 0xC0000000;
    _start = .;

   .text : AT(ADDR(.text) - 0xC0000000) {
       *(.text)
   }
 
   .data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000) {
       *(.data)
   }
 
   .bss : AT(ADDR(.bss) - 0xC0000000) {
		*(COMMON)
		*(.bss)
        *(.bootstrap_stack)
   }

    .rodata : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata)
    }

   _end = .;
}
