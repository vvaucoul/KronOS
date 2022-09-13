OUTPUT_FORMAT(elf32-i386)
ENTRY(low_kernel_entry)

SECTIONS
{
	__align_size = 0x1000;
	__kernel_physical_memory_start = 0x00100000; /* Start Kernel at 1MB */
	__kernel_virtual_memory_start = 0xC0000000; /* Start Kernel at 3GB */

    /******************************************************************************
    *                              BOOTING SECTTIONS                              *
    ******************************************************************************/

	. = __kernel_physical_memory_start;
	__multiboot_start = .;

	.multiboot.text : ALIGN(__align_size)
	{
		__multiboot_text_start = .;
		*(.multiboot.text)
		__multiboot_text_end = .;
	}

	.multiboot.bss : ALIGN(__align_size)
	{
		__multiboot_bss_start = .;
		*(.multiboot.bss)
		__multiboot_bss_end = .;
	}
	.multiboot.data : ALIGN(__align_size)
	{
		__multiboot_data_start = .;
		*(.multiboot.data)
		__multiboot_data_end = .;
	}
	.multiboot.rodata : ALIGN(__align_size)
	{
		__multiboot_rodata_start = .;
		*(.multiboot.rodata)
		__multiboot_rodata_end = .;
	}

	__multiboot_bss =  __multiboot_bss_start - __multiboot_start + 0x00100000;
	__multiboot_end_addr = . - __multiboot_start + 0x00100000;
	__multiboot_start_addr = __multiboot_start - __kernel_physical_memory_start + 0x00100000;
	__multiboot_end = .;

    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	. += __kernel_virtual_memory_start;
	__kernel_start = .;

	.text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000)
	{
		__kernel_text_section_start = .;
		*(.multiboot)
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
	__kernel_bss =  __kernel_bss_section_start - 0xC0000000 + 0x00100000;
	__kernel_end_addr = . - 0xC0000000 + 0x00100000;
	__kernel_start_addr = low_kernel_entry - __kernel_start + 0x00100000;
}