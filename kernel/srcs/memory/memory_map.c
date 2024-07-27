/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:18:24 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 08:34:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory_map.h>
#include <assert.h>

kernel_memory_map_t kernel_memory_map;
memory_map_t memory_map[MMAP_SIZE];

static memory_map_t __setup_memory_entry(MultibootMemoryType type, uint32_t size, uint32_t addr_low, uint32_t addr_high, uint32_t len_low, uint32_t len_high) {
    memory_map_t mmap;

    mmap.type = type;
    mmap.size = size;
    mmap.addr_low = addr_low;
    mmap.addr_high = addr_high;
    mmap.len_low = len_low;
    mmap.len_high = len_high;
    return (mmap);
}

static void __fix_memory_entry(memory_map_t *__memory_map) {
    __memory_map->addr_high = __memory_map->addr_low + __memory_map->len_low;
}

static int __init_memory_map(MultibootInfo *multiboot_info) {
    MultibootMemoryMap *mmap = NULL;

    uint32_t i = 0;
    uint32_t mmap_index = 0;

    do {

#if __HIGHER_HALF_KERNEL__ == true
        mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + i + 0xC0000000);
#else
        mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + i);
#endif

        assert(mmap != NULL);

        if (mmap->type > MMAP_MIN_TYPE && mmap->type <= MMAP_MAX_TYPE) {
            memory_map[mmap_index] = __setup_memory_entry(mmap->type, mmap->size, mmap->addr_low, mmap->addr_high, mmap->len_low, mmap->len_high);
            __fix_memory_entry(&memory_map[mmap_index]);

            if (mmap_index == 5) {
                memory_map[mmap_index].len_low = 0xFFFFFFFF - memory_map[mmap_index].addr_low;
            }
            ++mmap_index;
        }
        i += sizeof(MultibootMemoryMap);
    } while (i < multiboot_info->mmap_length);

    return (0);
}

int get_memory_map(MultibootInfo *multiboot_info) {
    assert(multiboot_info != NULL);

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