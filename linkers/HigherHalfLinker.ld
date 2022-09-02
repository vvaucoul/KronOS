OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
/* ENTRY(_kernel_entry) */

__align_size = 0x1000;
__Kernel_Memory_Start = 0xC0000000;
/* __Kernel_Memory_Start = 0x0; */

SECTIONS
{
    /******************************************************************************
    *                              BOOTING SECTTIONS                              *
    ******************************************************************************/

	. = 0x00100000;
	.multiboot.text BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.multiboot.text)
	}
	.multiboot.stack BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.multiboot.stack)
	}
	.multiboot.data BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.multiboot.data)
	}
	.multiboot.rodata BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.multiboot.rodata)
	}

    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	. += 0xC0000000;
	/* _kernel_start = .; */

    .text BLOCK(__align_size) : AT(ADDR(.text) - __Kernel_Memory_Start) ALIGN(__align_size)
	{
		_code_section = .;
		*(.multiboot)
		*(.text)
	}
	/* Read-only data. */
	.rodata BLOCK(__align_size) : AT(ADDR(.rodata) - __Kernel_Memory_Start) ALIGN(__align_size)
	{
		_rodata_section = .;
		*(.rodata)
	}
	/* Read-write data (initialized) */
	.data BLOCK(__align_size) : AT(ADDR(.data) - __Kernel_Memory_Start) ALIGN(__align_size)
	{
		_data_section = .;
		*(.data)
	}
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(__align_size) : AT(ADDR(.bss) - __Kernel_Memory_Start) ALIGN(__align_size)
	{
		_bss_section = .;
		*(COMMON)
		*(.bss)
	}

	_kernel_end = .;
}