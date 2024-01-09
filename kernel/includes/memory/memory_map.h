/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:16:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <kernel.h>
#include <multiboot/multiboot.h>
#include <system/sections.h>

typedef struct s_kernel_memory_map
{
    /* Kernel Sections */
    struct
    {
        /* Kernel Section */
        struct
        {
            uint32_t kernel_start;
            uint32_t kernel_end;
            uint32_t kernel_length;
        } kernel;

        /* Kernel Text Section */
        struct
        {
            uint32_t text_addr_start;
            uint32_t text_addr_end;
            uint32_t text_length;
        } text;

        /* Kernel Rodata Section */
        struct
        {
            uint32_t rodata_addr_start;
            uint32_t rodata_addr_end;
            uint32_t rodata_length;
        } rodata;

        /* Kernel Data Section */
        struct
        {
            uint32_t data_addr_start;
            uint32_t data_addr_end;
            uint32_t data_length;
        } data;

        /* Kernel BSS Section */
        struct
        {
            uint32_t bss_addr_start;
            uint32_t bss_addr_end;
            uint32_t bss_length;
        } bss;
    } sections;

    /* Kernel Length Section */
    struct
    {
        uint32_t total_memory_length;
    } total;
} kernel_memory_map_t;

typedef struct s_memory_map
{
    MultibootMemoryType type;
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
} memory_map_t;

#define MMAP_SIZE 0x7
#define MMAP_MIN_TYPE 0x0
#define MMAP_MAX_TYPE 0x5

extern kernel_memory_map_t kernel_memory_map;
extern memory_map_t memory_map[MMAP_SIZE];

#define KMAP_SECTIONS kernel_memory_map.sections
#define KMAP_TOTAL kernel_memory_map.total

extern int get_memory_map(MultibootInfo *multiboot_info);

#endif /* !MEMORY_MAP_H */