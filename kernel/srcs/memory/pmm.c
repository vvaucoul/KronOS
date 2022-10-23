/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 20:21:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/23 20:32:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

PMM_INFO __pmm_info;

static void pmm_set_block(pmm_physical_addr_t bit)
{
    __pmm_info.blocks[bit / PMM_BITS_ALIGN] |= (0x1 << (bit % PMM_BITS_ALIGN));
}

static void pmm_unset_block(pmm_physical_addr_t bit)
{
    __pmm_info.blocks[bit / PMM_BITS_ALIGN] &= ~(0x1 << (bit % PMM_BITS_ALIGN));
}

static bool pmm_test_block(pmm_physical_addr_t bit)
{
    return (__pmm_info.blocks[bit / PMM_BITS_ALIGN] & (0x1 << (bit % PMM_BITS_ALIGN)));
}

/*
** Find the first free frame and return its address
*/
static uint32_t pmm_get_first_free_block(void)
{
    for (uint32_t i = 0; i < pmm_get_max_blocks() / PMM_BITS_ALIGN; i++)
    {
        if (__pmm_info.blocks[i] != 0xFFFFFFFF)
        {
            for (uint32_t j = 0; j < PMM_BITS_ALIGN; j++)
            {
                uint32_t bit = 0x1 << j;
                if ((__pmm_info.blocks[i] & bit) == 0)
                    return (i * PMM_BITS_ALIGN + j);
            }
        }
    }
    return (PMM_FAILURE);
}

/*
** Find the first free frame with size: 'size' and return its address
*/
static uint32_t pmm_get_first_free_block_by_size(const uint32_t size)
{
    if (size == 0)
        return (PMM_FAILURE);
    else if (size == 1)
        return (pmm_get_first_free_block());
    else
    {
        for (uint32_t i = 0; i < pmm_get_max_blocks() / PMM_BITS_ALIGN; i++)
        {
            if (__pmm_info.blocks[i] != 0xFFFFFFFF)
            {
                for (uint32_t j = 0; j < PMM_BITS_ALIGN; j++)
                {
                    uint32_t bit = 0x1 << j;
                    if ((__pmm_info.blocks[i] & bit) == 0)
                    {
                        uint32_t sBit = (i * PMM_BITS_ALIGN) + bit;
                        uint32_t free = 0;
                        for (uint32_t count = 0; count <= size; count++)
                        {
                            if (pmm_test_block(sBit + count) == 0)
                                free++;
                            if (free == size)
                                return (i * PMM_BITS_ALIGN + j);
                        }
                    }
                }
            }
        }
    }
    return (PMM_FAILURE);
}

static void __pmm_init_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size)
{
    uint32_t align = base / PMM_BLOCK_SIZE;
    uint32_t blocks = size / PMM_BLOCK_SIZE;

    for (uint32_t i = 0; i < blocks; i++)
    {
        kprintf("Unset Block: %u | Used Blocks: %u\n", align + i, __pmm_info.infos.used_blocks);
        pmm_unset_block(align + i);
        __pmm_info.infos.used_blocks--;
    }
    pmm_set_block(0);
}

static void __pmm_deinit_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size)
{
    uint32_t align = base / PMM_BLOCK_SIZE;
    uint32_t blocks = size / PMM_BLOCK_SIZE;

    for (uint32_t i = 0; i < blocks; i++)
    {
        pmm_set_block(align + i);
        __pmm_info.infos.used_blocks++;
    }
}

static void *__pmm_alloc_block(void)
{
    if (pmm_get_free_blocks() == 0)
    {
        __PANIC("PMM: Out of Memory");
        return (PMM_NULL_ADDR);
    }
    uint32_t frame = pmm_get_first_free_block();

    if (frame == PMM_FAILURE)
    {
        __PANIC("PMM: Out of Memory");
        return (PMM_NULL_ADDR);
    }

    pmm_set_block(frame);
    pmm_physical_addr_t addr = frame * PMM_BLOCK_SIZE;
    __pmm_info.infos.used_blocks++;
    return ((void *)addr);
}

static void __pmm_free_block(void *ptr)
{
    pmm_physical_addr_t addr = (pmm_physical_addr_t)ptr;
    uint32_t frame = addr / PMM_BLOCK_SIZE;
    pmm_unset_block(frame);
    __pmm_info.infos.used_blocks--;
}

static void *__pmm_alloc_blocks(const uint32_t size)
{
    if (pmm_get_free_blocks() == 0)
    {
        __PANIC("PMM: Out of Memory");
        return (PMM_NULL_ADDR);
    }
    uint32_t frame = pmm_get_first_free_block_by_size(size);

    if (frame == PMM_FAILURE)
    {
        __PANIC("PMM: Out of Memory");
        return (PMM_NULL_ADDR);
    }

    for (uint32_t i = 0; i < size; i++)
    {
        pmm_set_block(frame + i);
        __pmm_info.infos.used_blocks++;
    }
    pmm_physical_addr_t addr = frame * PMM_BLOCK_SIZE;
    return ((void *)addr);
}

static void __pmm_free_blocks(void *ptr, const uint32_t size)
{
    pmm_physical_addr_t addr = (pmm_physical_addr_t)ptr;
    uint32_t frame = addr / PMM_BLOCK_SIZE;

    for (uint32_t i = 0; i < size; i++)
    {
        pmm_unset_block(frame + i);
        __pmm_info.infos.used_blocks--;
    }
}

static void __pmm_init(const pmm_physical_addr_t bitmap, const uint32_t total_memory_size)
{
    __pmm_info.infos.memory_size = total_memory_size;
    __pmm_info.infos.max_blocks = (pmm_get_memory_size()) / PMM_BLOCK_SIZE;
    __pmm_info.infos.used_blocks = pmm_get_max_blocks(),
    __pmm_info.infos.memory_map_start = bitmap;
    __pmm_info.blocks = (uint32_t *)bitmap;

    uint32_t blocks_set = pmm_get_max_blocks() / PMM_BITS_ALIGN * sizeof(uint32_t);

    kmemset(__pmm_info.blocks, PMM_DEFAULT_ADDR, blocks_set);

    __pmm_info.infos.memory_map_end = (uint32_t)&__pmm_info.blocks[pmm_get_max_blocks() / PMM_BITS_ALIGN];
    __pmm_info.infos.memory_map_length = pmm_get_memory_map_end() - pmm_get_memory_map_start();

    kprintf("PMM: Memory Size: %u MB\n", pmm_get_memory_size() / 1024);
    kprintf("PMM: Max Blocks: %u Blocks\n", pmm_get_max_blocks());
    kprintf("PMM: Used Blocks: %u Blocks\n", pmm_get_used_blocks());
    kprintf("PMM: Memory Map Start: 0x%x\n", pmm_get_memory_map_start());
    kprintf("PMM: Memory Map End: 0x%x\n", pmm_get_memory_map_end());
    kprintf("PMM: Memory Map Length: 0x%x\n", pmm_get_memory_map_length());
    kprintf("PMM: Memset Size %u Bytes with 0x%x\n", blocks_set, PMM_DEFAULT_ADDR);
}

/*******************************************************************************
 *                                PMM INTERFACE                                *
 ******************************************************************************/

/*
** PMM USEFULL INTERFACE FUNCTIONS
*/

pmm_info_t pmm_get_max_blocks(void)
{
    return (__pmm_info.infos.max_blocks);
}

pmm_info_t pmm_get_used_blocks(void)
{
    return (__pmm_info.infos.used_blocks);
}

pmm_info_t pmm_get_free_blocks(void)
{
    return (pmm_get_max_blocks() - pmm_get_used_blocks());
}

pmm_info_t pmm_get_memory_size(void)
{
    return (__pmm_info.infos.memory_size);
}

pmm_info_t pmm_get_memory_map_start(void)
{
    return (__pmm_info.infos.memory_map_start);
}

pmm_info_t pmm_get_memory_map_end(void)
{
    return (__pmm_info.infos.memory_map_end);
}

pmm_info_t pmm_get_memory_map_length(void)
{
    return (__pmm_info.infos.memory_map_length);
}

/*
** PMM INTERFACE FUNCTIONS
*/

void pmm_init_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size)
{
    __pmm_init_region(base, size);
}

void pmm_deinit_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size)
{
    __pmm_deinit_region(base, size);
}

uint32_t pmm_get_next_available_block(void)
{
    return (pmm_get_first_free_block());
}

uint32_t pmm_get_next_available_blocks(const uint32_t size)
{
    return (pmm_get_first_free_block_by_size(size));
}

void *pmm_alloc_block(void)
{
    return (__pmm_alloc_block());
}

void pmm_free_block(void *ptr)
{
    __pmm_free_block(ptr);
}

void *pmm_alloc_blocks(const uint32_t size)
{
    return (__pmm_alloc_blocks(size));
}

void pmm_free_blocks(void *ptr, const uint32_t size)
{
    __pmm_free_blocks(ptr, size);
}

void pmm_init(const pmm_physical_addr_t bitmap, const uint32_t total_memory_size)
{
    __pmm_init(bitmap, total_memory_size);
}