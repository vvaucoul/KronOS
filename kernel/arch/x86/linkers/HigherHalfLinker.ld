OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

__kernel_physical_memory_start = 0x00100000; /* Start Kernel at 1MB (Physical address space) */
__kernel_virtual_memory_start =  0xC0100000; /* Start Kernel at 3GB + 1MB (Virtual address space) */

SECTIONS
{
    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	__kernel_physical_memory_end = .;

	. = __kernel_virtual_memory_start;
	__kernel_section_start = .;

	.text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000) ALIGN(4K)
	{
		__kernel_text_section_start = .;
		*(.multiboot)
		*(.text)
		__kernel_text_section_end = .;
	}
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000) ALIGN(4K)
	{
		__kernel_rodata_section_start = .;
       *(.rodata*)
		__kernel_rodata_section_end = .;
	}
	.bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000) ALIGN(4K)
	{
		__kernel_bss_section_start = .;
		*(.COMMON)
		*(.bss)
        *(.initial_stack)
		__kernel_bss_section_end = .;
	}
	.data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000) ALIGN(4K)
	{
		__kernel_data_section_start = .;
		*(.data)
		__kernel_data_section_end = .;
	}

	__kernel_section_end = .;
	__kernel_virtual_memory_end = .;
}