/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm_loader.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:31:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/27 18:07:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <multiboot/multiboot.h>
#include <memory/pmm.h>
#include <memory/memory_map.h>
#include <memory/paging.h>
#include <system/panic.h>

static void __pmm_loader_reset_bitmap(void)
{
    for (uint32_t i = 0; i < pmm_get_max_blocks(); ++i)
    {
        __pmm_info.blocks[i] = 0b00000000;
    }
}

static void __pmm_loader_init_memory_bitmap(const pmm_physical_addr_t bitmap, const uint32_t total_memory_size)
{
    uint32_t frame = (uint32_t)bitmap >> 12;
    uint32_t length = total_memory_size;

    frame = frame >> 12;
    while (length % PMM_BLOCK_PER_BYTE)
    {
        __pmm_info.blocks[PMM_GET_BITMAP_ADDR(frame << 12)] |= (1 << (frame % PMM_BLOCK_PER_BYTE));
        ++frame;
        --length;
    }
    while (length != 0)
    {
        __pmm_info.blocks[PMM_GET_BITMAP_ADDR(frame << 12)] = 0xFFFFFFFF;
        frame += PMM_BLOCK_PER_BYTE;
        length -= PMM_BLOCK_PER_BYTE;
    }
}

static void __pmm_loader_init_kernel_region(void)
{
    uint32_t __start = __kernel_section_start;
    uint32_t __end = PMM_ALIGN_PAGE_SIZE(__kernel_section_end);
    for (uint32_t i = __start; i < __end; i++)
    {
        __pmm_info.blocks[PMM_GET_BITMAP_ADDR(i)] |= (1 << (i % PMM_BLOCK_PER_BYTE));
    }
}

static void __pmm_loader_init_memory_regions()
{
    for (uint8_t i = 0; i < __MEMORY_MAP_SIZE; i++)
    {
        const MultibootMemoryMap *current_map = &memory_map.map[i];
        uint32_t addr = 0;
        uint32_t length = 0;

        if (current_map->type != __MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        else if (current_map->addr_low >= MEMORY_GRUB_RESERVED_SPACE)
            continue;
        else if ((uint64_t)current_map->addr_low + current_map->len_low >= (uint64_t)MEMORY_GRUB_RESERVED_SPACE)
            continue;
    
        uint32_t __diff_addr = (uint32_t)current_map->addr_low % PMM_BLOCK_SIZE;

        /* Align addr */
        if (__diff_addr != 0)
        {
            addr = (uint32_t)current_map->addr_low + __diff_addr;
            length = (uint32_t)current_map->len_low - __diff_addr;
        }
        else
        {
            addr = (uint32_t)current_map->addr_low;
            length = (uint32_t)current_map->len_low;
        }

        uint32_t __diff_length = (uint32_t)current_map->len_low % PMM_BLOCK_SIZE;

        /* Align length */
        if (__diff_length != 0)
        {
            length = (uint32_t)current_map->len_low - __diff_length;
        }

        if (length < PMM_BLOCK_SIZE)
            continue;

        kprintf(" - Init memory region : "COLOR_GREEN"0x%x"COLOR_END" - "COLOR_GREEN"0x%x"COLOR_END" ("COLOR_GREEN"%u"COLOR_END" Kb)\n", addr, addr + length, length / 1024);
        pmm_init_region(addr, length);
    }
}

static void __pmm_loader_init(void)
{
    __pmm_loader_reset_bitmap();
    __pmm_loader_init_memory_regions();
    __pmm_loader_init_kernel_region();
}

void pmm_loader_init(void)
{
    __pmm_loader_init();
}