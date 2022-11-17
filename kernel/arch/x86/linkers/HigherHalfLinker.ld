OUTPUT_FORMAT(elf32-i386)
ENTRY(lower_half_kernel)

SECTIONS
{
	__align_size = 0x1000; /* Align to 4K (Page-Align) */
	__kernel_physical_memory_start = 0x00100000; /* Start Kernel at 1MB (Physical address space) */
	__kernel_virtual_memory_start = 0xC0000000; /* Start Kernel at 3GB + 1MB (Virtual address space) */

    /******************************************************************************
    *                              BOOTING SECTTIONS                              *
    ******************************************************************************/

	. = __kernel_physical_memory_start;
	__multiboot_start = .;

	.multiboot.text : 
	{
		__multiboot_text_start = .;
		*(.multiboot.text)
		__multiboot_text_end = .;
	}
	.multiboot.data : 
	{
		__multiboot_data_start = .;
		*(.multiboot.data)
		__multiboot_data_end = .;
	}
	
	__multiboot_end = .;

    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	. += __kernel_virtual_memory_start;
	__kernel_section_start = .;

	.text : AT(ADDR(.text) - 0xC0000000)
	{
		__kernel_text_section_start = .;
		/* *(.multiboot) */
		*(.text)
		__kernel_text_section_end = .;
	}
	.rodata : AT(ADDR(.rodata) - 0xC0000000)
	{
		__kernel_rodata_section_start = .;
       *(.rodata*)
		__kernel_rodata_section_end = .;
	}
	.data ALIGN (__align_size) : AT(ADDR(.data) - 0xC0000000)
	{
		__kernel_data_section_start = .;
		*(.data)
		__kernel_data_section_end = .;
	}
	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		__kernel_bss_section_start = .;
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		__kernel_bss_section_end = .;
	}

	__kernel_section_end = .;
	__kernel_virtual_memory_end = .;
	__kernel_physical_memory_end = .;

	. = ASSERT(__kernel_section_end - __kernel_section_start <= __kernel_physical_memory_start, "Kernel size too large");
}