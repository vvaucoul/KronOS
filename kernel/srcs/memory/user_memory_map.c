/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user_memory_map.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 20:39:13 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/23 20:45:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <memory/memory_map.h>
#include <memory/memory.h>

USER_MEMORY_MAP user_memory_map;

static int __init_user_mmap(const MultibootInfo *multiboot_info)
{
    for (uint32_t i = 0; i < multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE; i += sizeof(MultibootMemoryMap))
    {
        MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + KERNEL_VIRTUAL_BASE + i);
        if (mmap->type != __MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        else
        {
            UMAP.available.start_addr = (UMAP.user.user_end + 1024 * 1024) - KERNEL_VIRTUAL_BASE;
            UMAP.available.end_addr = mmap->addr_low + mmap->len_low;
            UMAP.available.length = UMAP.available.end_addr - UMAP.available.start_addr;
            return (0);
        }
    }
    return (1);
}

int get_user_memory_map(const MultibootInfo *multiboot_info)
{
    assert(multiboot_info == NULL);
    assert((&user_memory_map.user) == NULL);

    UMAP.user.user_start = (uint32_t)&__kernel_section_start - KERNEL_VIRTUAL_BASE;
    UMAP.user.user_end = (uint32_t)&__kernel_section_end - KERNEL_VIRTUAL_BASE;
    UMAP.user.user_length = UMAP.user.user_end - UMAP.user.user_start;

    UMAP.text.text_addr_start = (uint32_t)&__kernel_text_section_start - KERNEL_VIRTUAL_BASE;
    UMAP.text.text_addr_end = (uint32_t)&__kernel_text_section_end - KERNEL_VIRTUAL_BASE;
    UMAP.text.text_length = UMAP.text.text_addr_end - UMAP.text.text_addr_start;

    UMAP.rodata.rodata_addr_start = (uint32_t)&__kernel_rodata_section_start - KERNEL_VIRTUAL_BASE;
    UMAP.rodata.rodata_addr_end = (uint32_t)&__kernel_rodata_section_end - KERNEL_VIRTUAL_BASE;
    UMAP.rodata.rodata_length = UMAP.rodata.rodata_addr_end - UMAP.rodata.rodata_addr_start;

    UMAP.data.data_addr_start = (uint32_t)&__kernel_data_section_start - KERNEL_VIRTUAL_BASE;
    UMAP.data.data_addr_end = (uint32_t)&__kernel_data_section_end - KERNEL_VIRTUAL_BASE;
    UMAP.data.data_length = UMAP.data.data_addr_end - UMAP.data.data_addr_start;

    UMAP.bss.bss_addr_start = (uint32_t)&__kernel_bss_section_start - KERNEL_VIRTUAL_BASE;
    UMAP.bss.bss_addr_end = (uint32_t)&__kernel_bss_section_end - KERNEL_VIRTUAL_BASE;
    UMAP.bss.bss_length = UMAP.bss.bss_addr_end - UMAP.bss.bss_addr_start;

    UMAP.total.total_memory_length = multiboot_info->mem_upper + multiboot_info->mem_lower - KERNEL_VIRTUAL_BASE;

    if ((__init_user_mmap(multiboot_info)) == 1)
        return (1);
    return (0);
}
