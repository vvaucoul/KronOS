/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 13:28:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 19:40:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <kernel.h>

#define MULTIBOOT_MAGIC_HEADER 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

typedef enum e_multiboot_memory_type
{
    __MULTIBOOT_MEMORY_AVAILABLE = 1,
    __MULTIBOOT_MEMORY_RESERVED = 2,
    __MULTIBOOT_MEMORY_ACPI_RECLAIMABLE = 3,
    __MULTIBOOT_MEMORY_NVS = 4,
    __MULTIBOOT_MEMORY_BADRAM = 5
} multiboot_memory_type;

typedef struct s_multiboot_memory_map
{
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    enum e_multiboot_memory_type type;
} t_multiboot_memory_map;

/* The Multiboot header. */
typedef struct s_multiboot_header
{
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} t_multiboot_header;

/* The symbol table for a.out. */
typedef struct s_aout_symbol_table
{
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr;
    uint32_t reserved;
} t_aout_symbol_table;

/* The section header table for ELF. */
typedef struct s_multiboot_elf_section
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} t_multiboot_elf_section;

typedef struct s_multiboot_info
{
    /* required, defined in entry.asm */
    uint32_t flags;

    /* available low-high memory from BIOS, present if flags[0] is set(MEMINFO in entry.asm) */
    uint32_t mem_low;
    uint32_t mem_high;

    /* "root" partition, present if flags[1] is set(BOOTDEVICE in entry.asm) */
    uint32_t boot_device;

    /* kernel command line, present if flags[2] is set(CMDLINE in entry.asm) */
    uint32_t cmdline;

    /* no of modules loaded, present if flags[3] is set(MODULECOUNT in entry.asm) */
    uint32_t modules_count;
    uint32_t modules_addr;

    /* symbol table info, present if flags[4] & flags[5] is set(SYMT in entry.asm) */
    union
    {
        t_aout_symbol_table aout_sym;
        t_multiboot_elf_section elf_sec;
    } u;

    /* memory mapping, present if flags[6] is set(MEMMAP in entry.asm) */
    uint32_t mmap_length;
    uint32_t mmap_addr;

    /* drive info, present if flags[7] is set(DRIVE in entry.asm) */
    uint32_t drives_length;
    uint32_t drives_addr;

    /* ROM configuration table, present if flags[8] is set(CONFIGT in entry.asm) */
    uint32_t config_table;

    /* boot loader name, present if flags[9] is set(BOOTLDNAME in entry.asm) */
    uint32_t boot_loader_name;

    /* Advanced Power Management(APM) table, present if flags[10] is set(APMT in entry.asm) */
    uint32_t apm_table;

    /* video info, present if flags[11] is set(VIDEO in entry.asm) */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    /* video framebufer info, present if flags[12] is set(VIDEO_FRAMEBUF in entry.asm)  */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type; // indexed = 0, RGB = 1, EGA = 2
} t_multiboot_info;

#define MultibootMemoryMap t_multiboot_memory_map
#define MultibootInfo t_multiboot_info
#define MultibootHeader t_multiboot_header
#define MultibootELF t_multiboot_elf_section
#define MUltibootAout t_aout_symbol_table
#define MultibootMemoryType enum e_multiboot_memory_type

extern MultibootInfo *_multiboot_info;

static inline bool __check_magic_number(hex_t magic_number)
{
    if (magic_number != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHECK] " COLOR_RED "MAGIC NUMBER IS INVALID " COLOR_END "\n");
        UPDATE_CURSOR();
        return (false);
    }
    else
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHECK] " COLOR_CYAN "MAGIC NUMBER IS VALID " COLOR_END "\n");
    return (true);
}

static inline void __display_multiboot_infos(void)
{
    kprintf("  _multiboot_info: %p\n", &_multiboot_info);
    kprintf("  flags: 0x%x\n", _multiboot_info->flags);
    kprintf("  mem_low: 0x%x KB\n", _multiboot_info->mem_low);
    kprintf("  mem_high: 0x%x KB\n", _multiboot_info->mem_high);
    kprintf("  boot_device: 0x%x\n", _multiboot_info->boot_device);
    kprintf("  cmdline: %s\n", (char *)_multiboot_info->cmdline);
    return;
    kprintf("  modules_count: %d\n", _multiboot_info->modules_count);
    kprintf("  modules_addr: 0x%x\n", _multiboot_info->modules_addr);
    kprintf("  mmap_length: %d\n", _multiboot_info->mmap_length);
    kprintf("  mmap_addr: 0x%x\n", _multiboot_info->mmap_addr);
    kprintf("  memory map:-\n");
    for (uint32_t i = 0; i < _multiboot_info->mmap_length; i += sizeof(MultibootMemoryMap))
    {
        MultibootMemoryMap *mmap = (MultibootMemoryMap *)(_multiboot_info->mmap_addr + i);
        kprintf("    size: %d, addr: 0x%x%x, len: %d%d, type: %d\n",
                mmap->size, mmap->addr_low, mmap->addr_high, mmap->len_low, mmap->len_high, mmap->type);

        if (mmap->type == __MULTIBOOT_MEMORY_AVAILABLE)
        {
            /**** Available memory  ****/
        }
    }
    kprintf("  boot_loader_name: %s\n", (char *)_multiboot_info->boot_loader_name);
    kprintf("  vbe_control_info: 0x%x\n", _multiboot_info->vbe_control_info);
    kprintf("  vbe_mode_info: 0x%x\n", _multiboot_info->vbe_mode_info);
    kprintf("  framebuffer_addr: 0x%x\n", _multiboot_info->framebuffer_addr);
    kprintf("  framebuffer_width: %d\n", _multiboot_info->framebuffer_width);
    kprintf("  framebuffer_height: %d\n", _multiboot_info->framebuffer_height);
    kprintf("  framebuffer_type: %d\n", _multiboot_info->framebuffer_type);
}

#endif /* MULTIBOOT_H */