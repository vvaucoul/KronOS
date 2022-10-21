/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_memory_map.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 12:06:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 13:06:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <multiboot/multiboot.h>
#include <memory/memory_map.h>
#include <memory/paging.h>

KERNEL_MEMORY_MAP kernel_memory_map;

int get_kernel_memory_map(MultibootInfo *multiboot_info)
{
    if (multiboot_info == NULL)
        return (1);
    else if ((&kernel_memory_map.kernel) == NULL)
        return (1);

    KMAP.kernel.kernel_start = (uint32_t)&__kernel_section_start;
    KMAP.kernel.kernel_end = (uint32_t)&__kernel_section_end;
    KMAP.kernel.kernel_length = KMAP.kernel.kernel_end - KMAP.kernel.kernel_start;

    KMAP.text.text_addr_start = (uint32_t)&__kernel_text_section_start;
    KMAP.text.text_addr_end = (uint32_t)&__kernel_text_section_end;
    KMAP.text.text_length = KMAP.text.text_addr_end - KMAP.text.text_addr_start;

    KMAP.rodata.rodata_addr_start = (uint32_t)&__kernel_rodata_section_start;
    KMAP.rodata.rodata_addr_end = (uint32_t)&__kernel_rodata_section_end;
    KMAP.rodata.rodata_length = KMAP.rodata.rodata_addr_end - KMAP.rodata.rodata_addr_start;

    KMAP.data.data_addr_start = (uint32_t)&__kernel_data_section_start;
    KMAP.data.data_addr_end = (uint32_t)&__kernel_data_section_end;
    KMAP.data.data_length = KMAP.data.data_addr_end - KMAP.data.data_addr_start;

    KMAP.bss.bss_addr_start = (uint32_t)&__kernel_bss_section_start;
    KMAP.bss.bss_addr_end = (uint32_t)&__kernel_bss_section_end;
    KMAP.bss.bss_length = KMAP.bss.bss_addr_end - KMAP.bss.bss_addr_start;

    KMAP.total.total_memory_length = multiboot_info->mem_upper + multiboot_info->mem_lower;

    for (uint32_t i = 0; i < multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE; i += sizeof(MultibootMemoryMap))
    {
        MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + KERNEL_VIRTUAL_BASE + i);
        if (mmap->type != __MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        else
        {
            KMAP.available.start_addr = KMAP.kernel.kernel_end + 1024 * 1024;
            KMAP.available.end_addr = mmap->addr_low + mmap->len_low;
            KMAP.available.length = KMAP.available.end_addr - KMAP.available.start_addr;
            return (0);
        }
    }

    return (1);
}