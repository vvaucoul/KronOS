/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 13:28:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 13:01:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <kernel.h>

#define MULTIBOOT_SEARCH 8192
#define MULTIBOOT_HEADER_ALIGN 4

/* Check Multiboot flags */
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN 0x00001000

/* Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN 0x00000004

/* Flags set in the ’flags’ member of the multiboot header. */

/* Align all boot modules on i386 page (4KB) boundaries. */
#define MULTIBOOT_PAGE_ALIGN 0x00000001

/* Must pass memory information to OS. */
#define MULTIBOOT_MEMORY_INFO 0x00000002

/* Must pass video information to OS. */
#define MULTIBOOT_VIDEO_MODE 0x00000004

/* This flag indicates the use of the address fields in the header. */
#define MULTIBOOT_AOUT_KLUDGE 0x00010000

/* Flags to be set in the ’flags’ member of the multiboot info structure. */

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY 0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
/* is the ACPI_MEMORY defined? */
#define MULTIBOOT_ACPI_MEMORY 0x00000003
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE 0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS 0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS 0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR 0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP 0x00000040

/* Is there drive info? */
#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080

/* Is there a config table? */
#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100

/* Is there a boot loader name? */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200

/* Is there a APM table? */
#define MULTIBOOT_INFO_APM_TABLE 0x00000400

/* Is there video information? */
#define MULTIBOOT_INFO_VBE_INFO 0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

#ifndef __MULTIBOOT_UINT_8_T__
typedef unsigned char multiboot_uint8_t;
#define __MULTIBOOT_UINT_8_T__
#endif

#ifndef __MULTIBOOT_UINT_16_T__
typedef unsigned short multiboot_uint16_t;
#define __MULTIBOOT_UINT_16_T__
#endif

#ifndef __MULTIBOOT_UINT_32_T__
typedef unsigned int multiboot_uint32_t;
#define __MULTIBOOT_UINT_32_T__
#endif

#ifndef __MULTIBOOT_UINT_64_T__
typedef unsigned long long multiboot_uint64_t;
#define __MULTIBOOT_UINT_64_T__
#endif

/*
**         +-------------------+
** 0       | flags             |    (required)
**         +-------------------+
** 4       | mem_lower         |    (present if flags[0] is set)
** 8       | mem_upper         |    (present if flags[0] is set)
**         +-------------------+
** 12      | boot_device       |    (present if flags[1] is set)
**         +-------------------+
** 16      | cmdline           |    (present if flags[2] is set)
**         +-------------------+
** 20      | mods_count        |    (present if flags[3] is set)
** 24      | mods_addr         |    (present if flags[3] is set)
**         +-------------------+
** 28 - 40 | syms              |    (present if flags[4] or
**         |                   |                flags[5] is set)
**         +-------------------+
** 44      | mmap_length       |    (present if flags[6] is set)
** 48      | mmap_addr         |    (present if flags[6] is set)
**         +-------------------+
** 52      | drives_length     |    (present if flags[7] is set)
** 56      | drives_addr       |    (present if flags[7] is set)
**         +-------------------+
** 60      | config_table      |    (present if flags[8] is set)
**         +-------------------+
** 64      | boot_loader_name  |    (present if flags[9] is set)
**         +-------------------+
** 68      | apm_table         |    (present if flags[10] is set)
**         +-------------------+
** 72      | vbe_control_info  |    (present if flags[11] is set)
** 76      | vbe_mode_info     |
** 80      | vbe_mode          |
** 82      | vbe_interface_seg |
** 84      | vbe_interface_off |
** 86      | vbe_interface_len |
**         +-------------------+
** 88      | framebuffer_addr  |    (present if flags[12] is set)
** 96      | framebuffer_pitch |
** 100     | framebuffer_width |
** 104     | framebuffer_height|
** 108     | framebuffer_bpp   |
** 109     | framebuffer_type  |
** 110-115 | color_info        |
**         +-------------------+
*/

/* multiboot_header */
typedef struct s_multiboot_header
{
    /* Must be MULTIBOOT_MAGIC */
    multiboot_uint32_t magic;

    /* Feature flags */
    multiboot_uint32_t flags;

    /* The above fields plus this one must equal 0 mod 2^32. */
    multiboot_uint32_t checksum;

    /* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
    multiboot_uint32_t header_addr;
    multiboot_uint32_t load_addr;
    multiboot_uint32_t load_end_addr;
    multiboot_uint32_t bss_end_addr;
    multiboot_uint32_t entry_addr;

    /* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
    multiboot_uint32_t mode_type;
    multiboot_uint32_t width;
    multiboot_uint32_t height;
    multiboot_uint32_t depth;
} t_multiboot_header;

/* The symbol table for a.out. */
typedef struct s_aout_symbol_table
{
    multiboot_uint32_t tabsize;
    multiboot_uint32_t strsize;
    multiboot_uint32_t addr;
    multiboot_uint32_t reserved;
} t_aout_symbol_table;

/* The section header table for ELF. */
typedef struct s_elf_section_header_table
{
    multiboot_uint32_t num;
    multiboot_uint32_t size;
    multiboot_uint32_t addr;
    multiboot_uint32_t shndx;
} t_elf_section_header_table;

/* Multiboot memory information */
enum e_multiboot_memory_type
{
    __MULTIBOOT_MEMORY_AVAILABLE = 1,
    __MULTIBOOT_MEMORY_RESERVED = 2,
    __MULTIBOOT_MEMORY_ACPI_RECLAIMABLE = 3,
    __MULTIBOOT_MEMORY_NVS = 4,
    __MULTIBOOT_MEMORY_BADRAM = 5
};

/* A memory map entry. */
typedef struct s_multiboot_memory_map
{
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    enum e_multiboot_memory_type type;
} t_multiboot_memory_map;

typedef struct s_multiboot_info
{
    /* Multiboot info version number */
    uint32_t flags;

    /* Available memory from BIOS */
    uint32_t mem_lower;
    uint32_t mem_upper;

    /* "root" partition */
    uint32_t boot_device;

    /* Kernel command line */
    uint32_t cmdline;

    /* Boot-Module list */
    uint32_t mods_count;
    uint32_t mods_addr;

    union
    {
        t_aout_symbol_table aout_sym;
        t_elf_section_header_table elf_sec;
    } u;

    /* Memory Mapping buffer */
    uint32_t mmap_length;
    uint32_t mmap_addr;

    /* Drive Info buffer */
    uint32_t drives_length;
    uint32_t drives_addr;

    /* ROM configuration table */
    uint32_t config_table;

    /* Boot Loader Name */
    uint32_t boot_loader_name;

    /* APM table (Advanced Power Management) */
    uint32_t apm_table;

    /* VIDEO BIOS EXTENSIONS */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    /* Framebuffer */
    multiboot_uint64_t framebuffer_addr;
    multiboot_uint32_t framebuffer_pitch;
    multiboot_uint32_t framebuffer_width;
    multiboot_uint32_t framebuffer_height;
    multiboot_uint8_t framebuffer_bpp;

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2
    multiboot_uint8_t framebuffer_type;
    union
    {
        struct
        {
            multiboot_uint32_t framebuffer_palette_addr;
            multiboot_uint16_t framebuffer_palette_num_colors;
        };
        struct
        {
            multiboot_uint8_t framebuffer_red_field_position;
            multiboot_uint8_t framebuffer_red_mask_size;
            multiboot_uint8_t framebuffer_green_field_position;
            multiboot_uint8_t framebuffer_green_mask_size;
            multiboot_uint8_t framebuffer_blue_field_position;
            multiboot_uint8_t framebuffer_blue_mask_size;
        };
    };
} t_multiboot_info;

#define MultibootMemoryMap t_multiboot_memory_map
#define MultibootInfo t_multiboot_info
#define MultibootHeader t_multiboot_header
#define MultibootELF t_elf_section_header_table
#define MUltibootAout t_aout_symbol_table
#define MultibootMemoryType enum e_multiboot_memory_type

extern MultibootInfo *__multiboot_info;

extern int multiboot_init(MultibootInfo *mboot_ptr);
extern bool multiboot_check_magic_number(hex_t magic_number);

#endif /* MULTIBOOT_H */