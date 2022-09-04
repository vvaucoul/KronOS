OUTPUT_FORMAT(elf32-i386)
ENTRY(low_kernel_entry)

SECTIONS
{
	__align_size = 0x1000;
	__kernel_virtual_memory_start = 0xC0000000; /* Start Kernel at 3GB */
	__kernel_physical_memory_start = 0x00100000;

    /******************************************************************************
    *                              BOOTING SECTTIONS                              *
    ******************************************************************************/

	. = __kernel_physical_memory_start;
	__multiboot_start = .;

	.multiboot.text :
	{
		*(.multiboot.text)
	}
	.multiboot.bss :
	{
		*(.multiboot.bss)
	}
	.multiboot.data :
	{
		*(.multiboot.data)
	}
	.multiboot.rodata :
	{
		*(.multiboot.rodata)
	}

	__multiboot_end = .;

    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	. += __kernel_virtual_memory_start;
	__kernel_start = .;

	.text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000)
	{
		__kernel_text_section_start = .;
		*(.text)
		__kernel_text_section_end = .;
	}
	.data ALIGN (4096) : AT(ADDR(.data) - 0xC0000000)
	{
		__kernel_data_section_start = .;
		*(.data)
		__kernel_data_section_end = .;
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000)
	{
		__kernel_rodata_section_start = .;
       *(.rodata*)
		__kernel_rodata_section_end = .;
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - 0xC0000000)
	{
		__kernel_bss_section_start = .;
		*(COMMON)
		*(.bss)
		__kernel_bss_section_end = .;
	}
	__kernel_end = .;
}