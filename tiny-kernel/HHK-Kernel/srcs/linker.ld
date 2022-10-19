OUTPUT_FORMAT(elf32-i386)
ENTRY(lower_half_kernels)
 
SECTIONS {
    . = 0xC0100000;
    _start = .;

   .text : AT(ADDR(.text) - 0xC0000000) {
       *(.text)
       *(.rodata*)
   }
 
   .data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000) {
       *(.data)
   }
 
   .bss : AT(ADDR(.bss) - 0xC0000000) {
       _sbss = .;
       *(COMMON)
       *(.bss)
       _ebss = .;
   }

    .rodata : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata)
    }

   _end = .;
}