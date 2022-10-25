/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_memory_map.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 12:06:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/25 16:08:39 by vvaucoul         ###   ########.fr       */
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

static void __fix_memory_entries()
{
    // Fix Kernel ADDR
    memory_map.map[MEMORY_MAP_AVAILABLE_EXTENDED].addr_low += KERNEL_VIRTUAL_BASE;
    memory_map.map[MEMORY_MAP_AVAILABLE_EXTENDED].len_low = 0xFFFFFFFF - memory_map.map[MEMORY_MAP_AVAILABLE_EXTENDED].addr_low;

    // Fix Grub Reserved Space
    memory_map.map[MEMORY_MAP_GRUB_RESERVED].len_low = 0xFFFFFFFF - memory_map.map[MEMORY_MAP_GRUB_RESERVED].addr_low;
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

    __fix_memory_entries();

    for (uint8_t i = 0; i < memory_map.count; ++i)
    {
        // https://wiki.osdev.org/Memory_Map_(x86)
        MultibootMemoryMap *mmap = &memory_map.map[i];
        kprintf("Type: " COLOR_GREEN "%d" COLOR_END " | ", memory_map.map[i].type);

        kprintf("" COLOR_CYAN "0x%x" COLOR_END " -> " COLOR_CYAN "0x%x" COLOR_END "", MEMORY_MAP_GET_START_ADDR(memory_map.map, i), MEMORY_MAP_GET_END_ADDR(memory_map.map, i));
        kprintf(" | " COLOR_CYAN "%u" COLOR_END " - " COLOR_CYAN "%u" COLOR_END " - " COLOR_CYAN "%u" COLOR_END "\n", mmap->len_low, mmap->len_high, mmap->size);

        if (mmap->type == __MULTIBOOT_MEMORY_AVAILABLE)
        {
            kprintf("Addr Memory: 0x%x - 0x%x\n", mmap->addr_low, mmap->addr_low + mmap->len_low);
        }
        else if (mmap->type == __MULTIBOOT_MEMORY_RESERVED)
        {
            kprintf("Addr Memory: 0x%x - 0x%x\n", mmap->addr_low, mmap->addr_low + mmap->len_low);
        }
        // else
        // {
        //     kprintf("Unknown Memory: %u\n", mmap->len_low);
        // }
    }

    KMAP.available.start_addr = MEMORY_MAP_GET_START_ADDR(memory_map.map, MEMORY_MAP_AVAILABLE);
    KMAP.available.end_addr = MEMORY_MAP_ALIGN_ADDR(MEMORY_MAP_GET_END_ADDR(memory_map.map, MEMORY_MAP_AVAILABLE), 0x1000);
    KMAP.available.length = KMAP.available.end_addr - KMAP.available.start_addr;

    KMAP.available_extended.start_addr = MEMORY_MAP_GET_START_ADDR(memory_map.map, MEMORY_MAP_AVAILABLE_EXTENDED);
    KMAP.available_extended.end_addr = MEMORY_MAP_ALIGN_ADDR(MEMORY_MAP_GET_END_ADDR(memory_map.map, MEMORY_MAP_AVAILABLE_EXTENDED), 0x1000);
    KMAP.available_extended.length = KMAP.available_extended.end_addr - KMAP.available_extended.start_addr;

    kprintf("Kernel End ADDR: 0x%x\n", KMAP.sections.kernel.kernel_end);
    kprintf("Kernel Available Memory: 0x%x - 0x%x\n", KMAP.available.start_addr, KMAP.available.end_addr);
    kprintf("Kernel Available Memory Length: 0x%x\n", KMAP.available.length);

    kpause();
    return (0);
}

int get_kernel_memory_map(const MultibootInfo *multiboot_info)
{
    assert(multiboot_info == NULL);

    KMAP.sections.kernel.kernel_start = (uint32_t)&__kernel_section_start;
    KMAP.sections.kernel.kernel_end = (uint32_t)&__kernel_section_end;
    KMAP.sections.kernel.kernel_length = KMAP.sections.kernel.kernel_end - KMAP.sections.kernel.kernel_start;

    KMAP.sections.text.text_addr_start = (uint32_t)&__kernel_text_section_start;
    KMAP.sections.text.text_addr_end = (uint32_t)&__kernel_text_section_end;
    KMAP.sections.text.text_length = KMAP.sections.text.text_addr_end - KMAP.sections.text.text_addr_start;

    KMAP.sections.rodata.rodata_addr_start = (uint32_t)&__kernel_rodata_section_start;
    KMAP.sections.rodata.rodata_addr_end = (uint32_t)&__kernel_rodata_section_end;
    KMAP.sections.rodata.rodata_length = KMAP.sections.rodata.rodata_addr_end - KMAP.sections.rodata.rodata_addr_start;

    KMAP.sections.data.data_addr_start = (uint32_t)&__kernel_data_section_start;
    KMAP.sections.data.data_addr_end = (uint32_t)&__kernel_data_section_end;
    KMAP.sections.data.data_length = KMAP.sections.data.data_addr_end - KMAP.sections.data.data_addr_start;

    KMAP.sections.bss.bss_addr_start = (uint32_t)&__kernel_bss_section_start;
    KMAP.sections.bss.bss_addr_end = (uint32_t)&__kernel_bss_section_end;
    KMAP.sections.bss.bss_length = KMAP.sections.bss.bss_addr_end - KMAP.sections.bss.bss_addr_start;

    KMAP.total.total_memory_length = multiboot_info->mem_upper + multiboot_info->mem_lower;

    if ((__init_kernel_mmap(multiboot_info)) == 1)
        return (1);
    kpause();
    return (0);
}