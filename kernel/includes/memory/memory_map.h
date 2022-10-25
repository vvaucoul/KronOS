/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 20:19:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/25 15:28:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <kernel.h>
#include <multiboot/multiboot.h>

#define MEMORY_GRUB_RESERVED_SPACE 0xFFFFC000
#define MEMORY_PAGE_SCALABILITY 0x300

extern uint8_t __kernel_section_start;
extern uint8_t __kernel_text_section_start;
extern uint8_t __kernel_text_section_end;
extern uint8_t __kernel_data_section_start;
extern uint8_t __kernel_data_section_end;
extern uint8_t __kernel_rodata_section_start;
extern uint8_t __kernel_rodata_section_end;
extern uint8_t __kernel_bss_section_start;
extern uint8_t __kernel_bss_section_end;
extern uint8_t __kernel_section_end;

/*
** Kernel Memory Map
*/
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

    /* Kernel Available Section */
    struct
    {
        uint32_t start_addr;
        uint32_t end_addr;
        uint32_t length;
    } available;

    /* Kernel Available Extended Section */
    struct
    {
        uint32_t start_addr;
        uint32_t end_addr;
        uint32_t length;
    } available_extended;
} t_kernel_memory_map;


#define MEMORY_MAP_GET_START_ADDR(x, i) (x[i].addr_low)
#define MEMORY_MAP_GET_END_ADDR(x, i) (x[i].addr_low + x[i].len_low)
#define MEMORY_MAP_ALIGN_ADDR(x, size) (x & ~(size - 1))

#define __MEMORY_MAP_SIZE 7

#define MEMORY_MAP_LOW_MEMORY 0
#define MEMORY_MAP_LOW_UPPER_MEMORY 1
#define MEMORY_MAP_HARDWARE_RESERVED 2
#define MEMORY_MAP_AVAILABLE 3
#define MEMORY_MAP_ISA_MEMORY_HOLE 4
#define MEMORY_MAP_GRUB_RESERVED 5
#define MEMORY_MAP_AVAILABLE_EXTENDED 6

typedef struct __s_memory_map
{
    MultibootMemoryMap map[__MEMORY_MAP_SIZE];
    uint8_t count;
    uint8_t max_size;
} __t_memory_map;

#define KERNEL_MEMORY_MAP t_kernel_memory_map
#define MEMORY_MAP __t_memory_map

extern KERNEL_MEMORY_MAP kernel_memory_map;
extern MEMORY_MAP memory_map;

#define KMAP kernel_memory_map
#define MAP memory_map

extern int get_kernel_memory_map(const MultibootInfo *multiboot_info);
extern int get_user_memory_map(const MultibootInfo *multiboot_info);

#endif /* !MEMORY_MAP_H */