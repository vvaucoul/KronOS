OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
	__kernel_section_start = .;
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
	{
		__kernel_text_section_start = .;
		*(.multiboot)
		*(.text)
        . = ALIGN(4096);
		__kernel_text_section_end = .;
	}
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		__kernel_data_section_start = .;
		*(.data)
        . = ALIGN(4096);
		__kernel_data_section_end = .;
	}
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		__kernel_rodata_section_start = .;
		*(.rodata)
		__kernel_rodata_section_end = .;
	}
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		__kernel_bss_section_start = .;
		*(COMMON)
		*(.bss)
        . = ALIGN(4096);
		__kernel_bss_section_end = .;
	}
	__kernel_section_end = .;
}