OUTPUT_FORMAT(elf32-i386)
/* ENTRY(_start) */
ENTRY(_kernel_entry)

__align_size = 0x1000;

SECTIONS
{
    /******************************************************************************
    *                              BOOTING SECTTIONS                              *
    ******************************************************************************/

	. = 0x100000;
	/* .boot.text BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.boot.text)
	}
	.boot.stack BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.boot.stack)
	}
	.boot.data BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.boot.data)
	}
	.boot.rodata BLOCK(__align_size) : ALIGN(__align_size)
	{
		*(.boot.rodata)
	} */

    /******************************************************************************
    *                               KERNEL SECTIONS                               *
    ******************************************************************************/

	. += 0xC0000000;
	_kernel_start = .;

    .text BLOCK(__align_size) : AT(ADDR(.text) - 0xC0000000) ALIGN(__align_size)
	{
		_code_section = .;
		*(.multiboot)
		*(.text)
	}
	/* Read-only data. */
	.rodata BLOCK(__align_size) : AT(ADDR(.rodata) - 0xC0000000) ALIGN(__align_size)
	{
		_rodata_section = .;
		*(.rodata)
	}
	/* Read-write data (initialized) */
	.data BLOCK(__align_size) : AT(ADDR(.data) - 0xC0000000) ALIGN(__align_size)
	{
		_data_section = .;
		*(.data)
	}
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(__align_size) : AT(ADDR(.bss) - 0xC0000000) ALIGN(__align_size)
	{
		_bss_section = .;
		*(COMMON)
		*(.bss)
	}

	_kernel_end = .;
}