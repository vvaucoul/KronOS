;*******************************************************************************
;*                              MULTIBOOT STRUCT                               *
;*******************************************************************************

struc multiboot_info
	.flags              resd	1	; required
	.memory_low	        resd	1	; memory size. Present if flags[0] is set
	.memory_high        resd	1
	.boot_device        resd	1	; boot device. Present if flags[1] is set
	.cmdline	        resd	1	; kernel command line. Present if flags[2] is set
	.mods_count	        resd	1	; number of modules loaded along with kernel. present if flags[3] is set
	.mods_addr	        resd	1
	.syms0		        resd	1	; symbol table info. present if flags[4] or flags[5] is set
	.syms1		        resd	1
	.syms2		        resd	1
	.mmap_length	    resd	1	; memory map. Present if flags[6] is set
	.mmap_addr	        resd	1
	.drives_length	    resd	1	; phys address of first drive structure. present if flags[7] is set
	.drives_addr	    resd	1
	.config_table	    resd	1	; ROM configuation table. present if flags[8] is set
	.bootloader_name    resd	1	; Bootloader name. present if flags[9] is set
	.apm_table	        resd	1	; advanced power management (apm) table. present if flags[10] is set
	.vbe_control_info   resd	1	; video bios extension (vbe). present if flags[11] is set
	.vbe_mode_info	    resd	1
	.vbe_mode	        resw	1
	.vbe_interface_seg  resw	1
	.vbe_interface_off  resw	1
	.vbe_interface_len  resw	1
endstruc

; The Multiboot specification states that, when we invoke a 32 bit operating system (That is, execute our kernel), the machine registers must be set to a specific state. More specifically: When we execute our kernel, set up the registers to the following values:
; EAX - Magic Number. Must be 0x2BADB002. This will indicate to the kernel that our boot loader is multiboot standard
; EBX - Containes the physical address of the Multiboot information structure
; CS - Must be a 32-bit read/execute code segment with an offset of `0' and a limit of `0xFFFFFFFF'. The exact value is undefined.
; DS,ES,FS,GS,SS - Must be a 32-bit read/write data segment with an offset of `0' and a limit of `0xFFFFFFFF'. The exact values are all undefined.
; A20 gate must be enabled
; CR0 - Bit 31 (PG) bit must be cleared (paging disabled) and Bit 0 (PE) bit must be set (Protected Mode enabled). Other bits undefined
; All other registers are undefined. Most of this is already done in our existing boot loader. The only additional two things we must add are for the EAX register and EBX.
; The most important one for us is stored in EBX. This will contain the physical address of the multiboot information structure. Lets take a look!

MBOOT_BOOTLOADER_MAGIC equ 0x2BADB002

global __load_multiboot

__load_multiboot:
	; mov eax, MBOOT_BOOTLOADER_MAGIC
	; push dword multiboot_info
	; add	esp, 4
    ; cli