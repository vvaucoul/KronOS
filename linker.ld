OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

__kernel_virtual_memory_start = 0xc0100000;
__kernel_physical_memory_start = 0x00100000;

SECTIONS
{
    . = 1M;
	/* . = __kernel_virtual_memory_start;
	__kernel_start = .; */

	PROVIDE (_begin = .);

    .text BLOCK(4K) : ALIGN(4K)
	/* .text ALIGN (4K) : AT(ADDR(.text) - 0xC0000000) */
	{
		*(.multiboot)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	/* .rodata ALIGN (4K) : AT(ADDR(.rodata) - 0xC0000000) */
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	/* .data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000) */
	{
		*(.data)
	}
 
	PROVIDE (_edata = .);

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	/* .bss ALIGN (4K) : AT(ADDR(.bss) - 0xC0000000) */
	{
		*(COMMON)
		*(.bss)
	}

	PROVIDE (_end = .);
}

__kernel_virtual_memory_end = . ;
__kernel_physical_memory_end = . - 0xC0000000;