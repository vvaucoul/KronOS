;*******************************************************************************
;*                             EXTERNAL FUNCTIONS                              *
;*******************************************************************************

extern __load_multiboot

;*******************************************************************************
;*                          MULTIBOOT HEADER DEFINES                           *
;*******************************************************************************

MBOOT_ALIGN equ 1<< 0
MBOOT_MEMINFO equ 1<<1
MBOOT_CMDLINE equ 1<<2
MBOOT_MODULECOUNT equ 1<<3
MBOOT_SYMT equ 0x30 ; bits 4 & 5
MBOOT_MEMMAP equ 1<<6
MBOOT_DRIVE equ 1<<7
MBOOT_CONFIGT equ 1<<8
MBOOT_BOOTLDNAME equ 1<<9
MBOOT_APMT equ 1<<10
MBOOT_VIDEO equ 1<<11
MBOOT_VIDEO_FRAMEBUF equ 1<<12

; Bit 0: all boot modules loaded along with the operating system must be aligned on page (4KB) boundaries
MULTIBOOT_PAGE_ALIGN equ 1<<0
; Bit 1: information on available memory via at least the ‘mem_*’ fields of the Multiboot information structure
MULTIBOOT_MEMORY_INFO equ 1<<1
; Bit 2: information about the video mode table
MULTIBOOT_VIDEO_MODE equ 1<<2
; Bit 16: the fields at offsets 12-28 in the Multiboot header are valid, and the boot loader should use them instead of the fields
MULTIBOOT_AOUT_KLUDGE equ 1<<16

MBOOT_HEADER_FLAGS equ (MBOOT_ALIGN | MBOOT_MEMINFO)
; MBOOT_HEADER_FLAGS equ (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE | MULTIBOOT_AOUT_KLUDGE)
; MBOOT_HEADER_FLAGS equ 0x0
MBOOT_HEADER_MAGIC equ 0x1BADB002
MBOOT_CHECKSUM equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

;*******************************************************************************
;*                            KERNEL DEFINES VALUES                            *
;*******************************************************************************

KERNEL_STACK_SIZE equ 0x4000
KERNEL_PAGE_SIZE equ 0x1000

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22) ; Index in page Directory

PSE_BIT equ 0x00000010
PG_BIT equ 0x80000000

;*******************************************************************************
;*                      PMM / CONTIGUOUS PHYSICAL MEMORY                       *
;*******************************************************************************

PMM_VIRTUAL_BASE equ 0xB0000000
PMM_PAGE_NUMBER equ (PMM_VIRTUAL_BASE >> 22) ; Index in page Directory

;*******************************************************************************
;*                                CODE SECTIONS                                *
;*******************************************************************************

; Multiboot Section | For Grub
section .multiboot.data
align 4
; Declare a multiboot header that marks the program as a kernel.
multiboot:
	; Required fields
	; 0x0: u32: magic number
	dd MBOOT_HEADER_MAGIC
	; 0x4: u32: flags
	dd MBOOT_HEADER_FLAGS
	; 0x8: u32: checksum
	dd MBOOT_CHECKSUM

	; FLAG[16] MULTIBOOT_AOUT_KLUDGE
	; 0xC: u32: header_addr 
	dd 0x0
	; 0x10: u32: load_addr
	dd 0x0
	; 0x14: u32: load_end_addr
	dd 0x0
	; 0x18: u32: bss_end_addr
	dd 0x0
	; 0x1C: u32: entry_addr
	dd 0x0
	; FLAG[2] MBOOT_VIDEO_MODE
	; 0x20: u32: mode_type
	dd 0x0
	; 0x24: u32: width
	dd 0x0
	; 0x28: u32: height
	dd 0x0
	; 0x2C: u32: depth
	dd 0x0

	;0	u32	magic	required
	;4	u32	flags	required
	;8	u32	checksum	required
	;12	u32	header_addr	if flags[16] is set
	;16	u32	load_addr	if flags[16] is set
	;20	u32	load_end_addr	if flags[16] is set
	;24	u32	bss_end_addr	if flags[16] is set
	;28	u32	entry_addr	if flags[16] is set
	;32	u32	mode_type	if flags[2] is set
	;36	u32	width	if flags[2] is set
	;40	u32	height	if flags[2] is set
	;44	u32	depth	if flags[2] is set

; Allocate the initial stack.
section .bss, nobits ; does not contain any data
align 16
	stack_bottom:
	resb KERNEL_STACK_SIZE ; Kernel STACK = 16 KiB
	stack_top:

; Data Section
section .data
align 0x1000
KernelPageDirectory:
	dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0
   ; dd 0xB0000083 
    ;times (KERNEL_PAGE_NUMBER - PMM_PAGE_NUMBER - 1) dd 0 
    dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

; Text Section
section .text
	global kernel_entry
	global low_kernel_entry

low_kernel_entry: ;equ (kernel_entry - KERNEL_VIRTUAL_BASE)

kernel_entry:
	mov ecx, (KernelPageDirectory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

	; Enable 4mb pages
   	mov ecx, cr4
    or ecx, PSE_BIT
   	mov cr4, ecx

    ; Set PG bit, enable paging
    mov ecx, cr0
	or ecx, PG_BIT
    mov cr0, ecx

	lea ecx, [higher_half_kernel]
    jmp ecx

higher_half_kernel:
	mov dword[KernelPageDirectory], 0
    invlpg[0]

	mov esp, stack_top

	call __load_multiboot
	
	push ebx
	push eax
	
	extern kmain
	call kmain

.hang:
	hlt
	jmp .hang	
.end: