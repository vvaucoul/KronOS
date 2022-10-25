/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm_loader.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:31:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/24 18:00:20 by vvaucoul         ###   ########.fr       */
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
        __pmm_info.blocks[i] = 0x0;
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

static void __pmm_loader_init_memory_regions(const MultibootInfo *multiboot_info)
{
    MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + (uint32_t)KERNEL_VIRTUAL_BASE);

    do
    {
        mmap = (MultibootMemoryMap *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));

        kprintf("Mmap Type: %d\n", mmap->type);
        if (mmap->type == __MULTIBOOT_MEMORY_AVAILABLE)
        {
            kprintf("Available Memory: %u - %u - %u\n", mmap->len_low, mmap->len_high, mmap->size);
            kprintf("Addr Memory: 0x%x - 0x%x\n", mmap->addr_low, mmap->addr_high);
        }
        else if (mmap->type == __MULTIBOOT_MEMORY_RESERVED)
        {
            kprintf("Reserved Memory: %u - %u - %u\n", mmap->len_low, mmap->len_high, mmap->size);
            kprintf("Addr Memory: 0x%x - 0x%x\n", mmap->addr_low, mmap->addr_high);
        }
        else
        {
            kprintf("Unknown Memory: %u\n", mmap->len_low);
        }
    } while ((uint32_t)mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE);
}

void pmm_loader_init(const MultibootInfo *multiboot_info)
{
    __pmm_loader_reset_bitmap();
    __pmm_loader_init_memory_regions(multiboot_info);
    __pmm_loader_init_kernel_region();
    // __pmm_loader_init_memory_bitmap(multiboot_info->mmap_addr, multiboot_info->mem_upper);
}