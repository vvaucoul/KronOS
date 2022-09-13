/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 20:19:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 15:01:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <kernel.h>

extern uint8_t __kernel_section_start;
extern uint8_t __kernel_text_section_start;
extern uint8_t __kernel_text_section_end;
extern uint8_t __kernel_rodata_section_start;
extern uint8_t __kernel_rodata_section_end;
extern uint8_t __kernel_data_section_start;
extern uint8_t __kernel_data_section_end;
extern uint8_t __kernel_bss_section_start;
extern uint8_t __kernel_bss_section_end;
extern uint8_t __kernel_section_end;

typedef struct s_kernel_memory_map
{
    /* Kernel Section */
    struct {
        uint32_t kernel_start;
        uint32_t kernel_end;
        uint32_t kernel_length;
    } kernel;

    /* Kernel Text Section */
    struct {
        uint32_t text_addr_start;
        uint32_t text_addr_end;
        uint32_t text_length;
    } text;

    /* Kernel Rodata Section */
    struct {
        uint32_t rodata_addr_start;
        uint32_t rodata_addr_end;
        uint32_t rodata_length;
    } rodata;

    /* Kernel Data Section */
    struct {
        uint32_t data_addr_start;
        uint32_t data_addr_end;
        uint32_t data_length;
    } data;

    /* Kernel BSS Section */
    struct {
        uint32_t bss_addr_start;
        uint32_t bss_addr_end;
        uint32_t bss_length;
    } bss;

    /* Kernel Length Section */
    struct {
        uint32_t total_memory_length;
    } total;

    /* Kernel Available Section */
    struct {
        uint32_t start_addr;
        uint32_t end_addr;
        uint32_t length;
    } available;
} t_kernel_memory_map;

#define KERNEL_MEMORY_MAP t_kernel_memory_map
extern KERNEL_MEMORY_MAP kernel_memory_map;

#define KMAP kernel_memory_map

extern int get_kernel_memory_map(MultibootInfo *multiboot_info);
extern void display_kernel_memory_map(void);

#endif /* !MEMORY_MAP_H */