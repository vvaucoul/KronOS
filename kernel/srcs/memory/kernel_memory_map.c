/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_memory_map.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 12:06:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/25 13:38:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <memory/memory_map.h>
#include <memory/memory.h>

KERNEL_MEMORY_MAP kernel_memory_map;
MEMORY_MAP memory_map;

static MultibootMemoryMap __setup_memory_entry(MultibootMemoryType type, uint32_t size, uint32_t addr_low, uint32_t addr_high, uint32_t len_low, uint32_t len_high)
{
    MultibootMemoryMap mmap;
    mmap.type = type;
    mmap.size = size;
    mmap.addr_low = addr_low;
    mmap.addr_high = addr_high;
    mmap.len_low = len_low;
    mmap.len_high = len_high;
    return (mmap);
}

static int __init_kernel_mmap(const MultibootInfo *multiboot_info)
{
    // for (uint32_t i = 0; i < multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE; i += sizeof(MultibootMemoryMap))
    // {
    //     MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + KERNEL_VIRTUAL_BASE + i);
    //     if (mmap->type != __MULTIBOOT_MEMORY_AVAILABLE)
    //         continue;
    //     else
    //     {
    //         KMAP.available.start_addr = KMAP.kernel.kernel_end + 1024 * 1024;
    //         KMAP.available.end_addr =  mmap->addr_low + mmap->len_low;
    //         KMAP.available.length = KMAP.available.end_addr - KMAP.available.start_addr;
    //         return (0);
    //     }
    // }

    memory_map.max_size = __MEMORY_MAP_SIZE;
    memory_map.count = 0;
    for (uint8_t i = 0; i < __MEMORY_MAP_SIZE; ++i)
    {
        memory_map.map[i].type = 0x0;
        memory_map.map[i].size = 0x0;
        memory_map.map[i].addr_low = 0x0;
        memory_map.map[i].addr_high = 0x0;
        memory_map.map[i].len_low = 0x0;
        memory_map.map[i].len_high = 0x0;
    }

    MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + (uint32_t)KERNEL_VIRTUAL_BASE);
    do
    {
        memory_map.map[memory_map.count] = __setup_memory_entry(mmap->type, mmap->size, mmap->addr_low, mmap->addr_high, mmap->len_low, mmap->len_high);
        ++memory_map.count;
        mmap = (MultibootMemoryMap *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    } while ((uint32_t)mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE);

    for (uint8_t i = 0; i < memory_map.count; ++i)
    {
        // https://wiki.osdev.org/Memory_Map_(x86)
        MultibootMemoryMap *mmap = &memory_map.map[i];
        kprintf("Mmap Type: %d\n", memory_map.map[i].type);
        if (mmap->type == __MULTIBOOT_MEMORY_AVAILABLE)
        {
            kprintf("Available Memory: %u - %u - %u\n", mmap->len_low, mmap->len_high, mmap->size);
            kprintf("Addr Memory: 0x%x - 0x%x\n", mmap->addr_low, mmap->addr_low + mmap->len_low);

            KMAP.available.start_addr = KMAP.kernel.kernel_end;
            KMAP.available.end_addr = mmap->addr_low + mmap->len_low;
            KMAP.available.length = KMAP.available.end_addr - KMAP.available.start_addr;
        }
        else if (mmap->type == __MULTIBOOT_MEMORY_RESERVED)
        {
            kprintf("Reserved Memory: %u - %u - %u\n", mmap->len_low, mmap->len_high, mmap->size);
            kprintf("Addr Memory: 0x%x - 0x%x\n", mmap->addr_low, mmap->addr_low + mmap->len_low);
        }
        else
        {
            kprintf("Unknown Memory: %u\n", mmap->len_low);
        }
    }

    kpause();
    return (0);
}

int get_kernel_memory_map(const MultibootInfo *multiboot_info)
{
    assert(multiboot_info == NULL);

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

    if ((__init_kernel_mmap(multiboot_info)) == 1)
        return (1);
    kpause();
    return (0);
}