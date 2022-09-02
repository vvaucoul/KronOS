OUTPUT_FORMAT(elf32-i386)
ENTRY(_kernel_entry)

__align_size = 0x1000;
__Kernel_Memory_Start = 0xC0000000; /* Start Kernel at 3GB */

SECTIONS
{
    /******************************************************************************
    *                              BOOTING SECTTIONS                              *
    ******************************************************************************/

	. = 0x00100000;
	.multiboot.text :
	{
		*(.multiboot.text)
	}
	.multiboot.stack :
	{
		*(.multiboot.stack)
	}
	.multiboot.data :
	{
		*(.multiboot.data)
	}
	.multiboot.rodata :
	{
		*(.multiboot.rodata)
	}

    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	. += __Kernel_Memory_Start;
	_kernel_start = .;

    .text ALIGN(__align_size) : AT(ADDR(.text) - __Kernel_Memory_Start)
	{
		_code_section = .;
		/* *(.multiboot) */
		*(.text)
	}
	/* Read-only data. */
	.rodata ALIGN(__align_size) : AT(ADDR(.rodata) - __Kernel_Memory_Start)
	{
		_rodata_section = .;
		*(.rodata)
	}
	/* Read-write data (initialized) */
	.data ALIGN(__align_size) : AT(ADDR(.data) - __Kernel_Memory_Start)
	{
		_data_section = .;
		*(.data)
	}
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(__align_size) : AT(ADDR(.bss) - __Kernel_Memory_Start)
	{
		_bss_section = .;
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	_kernel_end = .;
}