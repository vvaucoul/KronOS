;*******************************************************************************
;*                             EXTERNAL FUNCTIONS                              *
;*******************************************************************************

extern __load_multiboot

;*******************************************************************************
;*                          MULTIBOOT HEADER DEFINES                           *
;*******************************************************************************

MBOOT_MEMINFO     equ  1<<0
MBOOT_BOOTDEVICE  equ  1<<1
MBOOT_CMDLINE     equ  1<<2
MBOOT_MODULECOUNT equ  1<<3
MBOOT_SYMT        equ  0x30 ; bits 4 & 5
MBOOT_MEMMAP      equ  1<<6
MBOOT_DRIVE       equ  1<<7
MBOOT_CONFIGT     equ  1<<8
MBOOT_BOOTLDNAME  equ  1<<9
MBOOT_APMT        equ  1<<10
MBOOT_VIDEO       equ  1<<11
MBOOT_VIDEO_FRAMEBUF equ  1<<12

; Bit 0: all boot modules loaded along with the operating system must be aligned on page (4KB) boundaries
MULTIBOOT_PAGE_ALIGN equ 0x00000001
; Bit 1: information on available memory via at least the ‘mem_*’ fields of the Multiboot information structure
MULTIBOOT_MEMORY_INFO equ 0x00000002
; Bit 2: information about the video mode table
MULTIBOOT_VIDEO_MODE equ 0x00000004
; Bit 16: the fields at offsets 12-28 in the Multiboot header are valid, and the boot loader should use them instead of the fields
MULTIBOOT_AOUT_KLUDGE equ 0x00010000

MBOOT_HEADER_FLAGS equ (MBOOT_MEMINFO | MBOOT_BOOTDEVICE | MULTIBOOT_VIDEO_MODE | MULTIBOOT_AOUT_KLUDGE)
MBOOT_HEADER_MAGIC equ 0x1BADB002
MBOOT_BOOTLOADER_MAGIC equ 0x2BADB002
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
;*                            LINKER EXTERN VALUES                             *
;*******************************************************************************

extern __align_size
extern __kernel_physical_memory_start
extern __kernel_virtual_memory_start
extern __kernel_virtual_memory_end
extern __kernel_section_start
extern __kernel_text_section_start
extern __kernel_text_section_end
extern __kernel_data_section_start
extern __kernel_data_section_end
extern __kernel_rodata_section_start
extern __kernel_rodata_section_end
extern __kernel_bss_section_start
extern __kernel_bss_section_end
extern __kernel_section_end
extern __kernel_bss
extern __kernel_end_addr
extern __kernel_start_addr

extern __multiboot_start
extern __multiboot_text_start
extern __multiboot_text_end
extern __multiboot_bss
extern __multiboot_bss_start
extern __multiboot_bss_end
extern __multiboot_data_start
extern __multiboot_data_end
extern __multiboot_rodata_start
extern __multiboot_rodata_end

extern __multiboot_end
extern __multiboot_end_addr
extern __multiboot_start_addr

;*******************************************************************************
;*                                CODE SECTIONS                                *
;*******************************************************************************

extern lhk_loader

; Multiboot Section | For Grub
global _start
_start:
	jmp low_kernel_entry

section .multiboot
align 4
; Declare a multiboot header that marks the program as a kernel.
multiboot_header:
	align 4
	; Required fields
	; 0x0: u32: magic number
	dd MBOOT_HEADER_MAGIC
	; 0x4: u32: flags
	dd MBOOT_HEADER_FLAGS
	; 0x8: u32: checksum
	dd MBOOT_CHECKSUM

; FLAG[16] MULTIBOOT_AOUT_KLUDGE
entry_addr_start:
	align 4
	; 0xC: u32: header_addr 
	dd 0x100000
	; 0x10: u32: load_addr
	dd 0x100000
	; 0x14: u32: load_end_addr  (load_end_addr - load_addr)
	dd __kernel_bss
	; 0x18: u32: bss_end_addr
	dd __kernel_end_addr
	; 0x1C: u32: entry_addr
	dd __kernel_start_addr
entry_addr_end:
	align 4
	; FLAG[2] MBOOT_VIDEO_MODE
	; 0x20: u32: mode_type
	dd 1
	; 0x24: u32: width
	dd 800
	; 0x28: u32: height
	dd 600
	; 0x2C: u32: depth
	dd 32

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
section .bss;, nobits ; does not contain any data
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
    dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

; Text Section
section .text
	global kernel_entry
	global low_kernel_entry

LHK_ERROR: dw "ERROR: LHK Loader !", 0

extern init_multiboot_kernel
extern BOOTLOADER_MAGIC


low_kernel_entry: 
	; mov esp, stack_top

	push ebx
	push eax

    call init_multiboot_kernel
	
	pop eax
	pop ebx

	jmp kernel_entry

	push ebx
	call lhk_loader
	cmp eax, 0
	jnz .error
	jmp kernel_entry

.error:
	extern display_error_msg

	; mov eax, [LHK_ERROR]
	call display_error_msg
	hlt

	; Display Error Message
	mov word [0xB8000], 0x0445
	mov word [0xB8002], 0x0452
	mov word [0xB8004], 0x0452
	mov word [0xB8006], 0x044F
	mov word [0xB8008], 0x0452
	mov word [0xB8010], 0x0F20
	mov word [0xB8012], 0x0F4B
	hlt

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
	mov eax, MBOOT_BOOTLOADER_MAGIC

	push ebx
	push eax
	
	extern kmain
	call kmain

.hang:
	hlt
	jmp .hang	
.end: