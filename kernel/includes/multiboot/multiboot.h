/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 13:28:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/26 21:57:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <kernel.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     DEFINES                                    ||
// ! ||--------------------------------------------------------------------------------||

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

#define MULTIBOOT_FLAGS 0x0
#define MULTIBOOT_INFO_MEMORY 0x00000001
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
#define MULTIBOOT_ACPI_MEMORY 0x00000003
#define MULTIBOOT_INFO_CMDLINE 0x00000004
#define MULTIBOOT_INFO_MODS 0x00000008

#define MULTIBOOT_INFO_DEVICE 0x00000001
#define MULTIBOOT_INFO_VBE 0x00000400

#define MULTIBOOT_INFO_AOUT_SYMS 0x00000010
#define MULTIBOOT_INFO_ELF_SHDR 0X00000020

#define MULTIBOOT_INFO_MEM_MAP 0x00000040

#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080

#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100

#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200

#define MULTIBOOT_INFO_APM_TABLE 0x00000400

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

// ! ||--------------------------------------------------------------------------------||
// ! ||                               MULTIBOOT STRUCTURE                              ||
// ! ||--------------------------------------------------------------------------------||

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

typedef struct
{
    uint16_t attributes;
    uint8_t winA, winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, segmentB;
    uint32_t realFctPtr;
    uint16_t pitch;

    uint16_t Xres, Yres;
    uint8_t Wchar, Ychar, planes, bpp, banks;
    uint8_t memory_model, bank_size, image_pages;
    uint8_t reserved0;

    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t rsv_mask, rsv_position;
    uint8_t directcolor_attributes;

    uint32_t physbase;
    uint32_t reserved1;
    uint16_t reserved2;
} __attribute__((packed)) vbe_info_t;

#define MultibootMemoryMap t_multiboot_memory_map
#define MultibootInfo t_multiboot_info
#define MultibootHeader t_multiboot_header
#define MultibootELF t_elf_section_header_table
#define MUltibootAout t_aout_symbol_table
#define MultibootMemoryType enum e_multiboot_memory_type

extern MultibootInfo *__multiboot_info;

extern int multiboot_init(MultibootInfo *mboot_ptr);
extern bool multiboot_check_magic_number(uint32_t magic_number);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     MACROS                                     ||
// ! ||--------------------------------------------------------------------------------||

#define CHECK_AND_LOG_FLAG(mboot_ptr, flag, name)                          \
    do                                                                     \
    {                                                                      \
        if (!CHECK_FLAG((mboot_ptr)->flags, (flag)))                       \
        {                                                                  \
            printk(_GREEN "[CHECK] " name ": " _RED "INVALID " _END "\n"); \
            return (1);                                                      \
        }                                                                  \
    } while (0)

#define GET_PHYSICAL_MEMORY_SIZE(mboot_ptr) ((mboot_ptr)->mem_upper + (mboot_ptr)->mem_lower)

#endif /* !MULTIBOOT_H */