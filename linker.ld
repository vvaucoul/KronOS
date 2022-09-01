OUTPUT_FORMAT(elf32-i386)
/* ENTRY(_start) */
ENTRY(_kernel_entry)

SECTIONS
{
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
	{
		_code_section = .;
		*(.multiboot)
		*(.text)
	}
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		_rodata_section = .;
		*(.rodata)
	}
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		_data_section = .;
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		_bss_section = .;
		*(COMMON)
		*(.bss)
	}
}