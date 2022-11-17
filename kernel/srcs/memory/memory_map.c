/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:18:24 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/17 17:35:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory_map.h>

kernel_memory_map_t kernel_memory_map;
memory_map_t memory_map[MMAP_SIZE];

static memory_map_t __setup_memory_entry(MultibootMemoryType type, uint32_t size, uint32_t addr_low, uint32_t addr_high, uint32_t len_low, uint32_t len_high)
{
    memory_map_t mmap;

    mmap.type = type;
    mmap.size = size;
    mmap.addr_low = addr_low;
    mmap.addr_high = addr_high;
    mmap.len_low = len_low;
    mmap.len_high = len_high;
    return (mmap);
}

static void __fix_memory_entry(memory_map_t *__memory_map)
{
    __memory_map->addr_high = __memory_map->addr_low + __memory_map->len_low;
}

static int __init_memory_map(MultibootInfo *multiboot_info)
{
    MultibootMemoryMap *mmap = (MultibootMemoryMap *)multiboot_info->mmap_addr;

    uint32_t index = 0;
    uint32_t mmap_index = 0;

    do
    {
        assert(mmap == NULL);
        if (mmap->type > MMAP_MIN_TYPE && mmap->type <= MMAP_MAX_TYPE)
        {
            memory_map[mmap_index] = __setup_memory_entry(mmap->type, mmap->size, mmap->addr_low, mmap->addr_high, mmap->len_low, mmap->len_high);
            __fix_memory_entry(&memory_map[mmap_index]);
            kprintf("Add Memory map [%u] -> %u\n", mmap_index, mmap->type);
            ++mmap_index;
        }
        mmap = (MultibootMemoryMap *)(uint32_t)multiboot_info->mmap_addr + index;
        index++;
    } while ((uint32_t)index < multiboot_info->mmap_length);

    for (uint32_t i = 0; i < MMAP_SIZE; i++)
    {
        const memory_map_t mmap = memory_map[i];
        kprintf("MMAP [%u]", i);
        kprintf(" |: [%d]", mmap.type);
        kprintf(" |: 0x%x", mmap.addr_low);
        kprintf(" |: 0x%x", mmap.addr_high);
        kprintf(" |: %u", mmap.len_low);
        kprintf(" |: %u", mmap.len_high);
        kprintf(" | Total: %u", mmap.addr_low + mmap.len_low);
        kprintf("\n");
    }

    return (0);
}

int get_memory_map(MultibootInfo *multiboot_info)
{
    assert(multiboot_info == NULL);

    KMAP_SECTIONS.kernel.kernel_start = (uint32_t)&__kernel_section_start;
    KMAP_SECTIONS.kernel.kernel_end = (uint32_t)&__kernel_section_end;
    KMAP_SECTIONS.kernel.kernel_length = KMAP_SECTIONS.kernel.kernel_end - KMAP_SECTIONS.kernel.kernel_start;

    KMAP_SECTIONS.text.text_addr_start = (uint32_t)&__kernel_text_section_start;
    KMAP_SECTIONS.text.text_addr_end = (uint32_t)&__kernel_text_section_end;
    KMAP_SECTIONS.text.text_length = KMAP_SECTIONS.text.text_addr_end - KMAP_SECTIONS.text.text_addr_start;

    KMAP_SECTIONS.rodata.rodata_addr_start = (uint32_t)&__kernel_rodata_section_start;
    KMAP_SECTIONS.rodata.rodata_addr_end = (uint32_t)&__kernel_rodata_section_end;
    KMAP_SECTIONS.rodata.rodata_length = KMAP_SECTIONS.rodata.rodata_addr_end - KMAP_SECTIONS.rodata.rodata_addr_start;

    KMAP_SECTIONS.data.data_addr_start = (uint32_t)&__kernel_data_section_start;
    KMAP_SECTIONS.data.data_addr_end = (uint32_t)&__kernel_data_section_end;
    KMAP_SECTIONS.data.data_length = KMAP_SECTIONS.data.data_addr_end - KMAP_SECTIONS.data.data_addr_start;

    KMAP_SECTIONS.bss.bss_addr_start = (uint32_t)&__kernel_bss_section_start;
    KMAP_SECTIONS.bss.bss_addr_end = (uint32_t)&__kernel_bss_section_end;
    KMAP_SECTIONS.bss.bss_length = KMAP_SECTIONS.bss.bss_addr_end - KMAP_SECTIONS.bss.bss_addr_start;

    KMAP_TOTAL.total_memory_length = multiboot_info->mem_upper + multiboot_info->mem_lower;

    if ((__init_memory_map(multiboot_info)) == 1)
        return (1);
    return (0);
}