/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 12:06:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 18:47:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/pmm.h>
#include <system/serial.h>

PMM_INFO pmm_info;

/*******************************************************************************
 *                            PRIVATE PMM FUNCTIONS                            *
 ******************************************************************************/

static int __pmm_init_blocks(uint32_t max_size)
{
    uint32_t i;
    PMM_BLOCK *tmp = NULL;
    PMM_BLOCK *prev = NULL;

    __PMM_GET_BLOCK(tmp);

    i = 0;
    while (i < max_size)
    {
        tmp->addr = PMM_NULL_ADDR;
        tmp->size = 0x0;
        tmp->id = i;
        tmp->state = PMM_BLOCK_FREE;
        tmp->list.next = NULL;
        tmp->list.prev = prev;

        prev = tmp;
        PMM_SHIFT_BLOCK(tmp);
        prev->list.next = tmp;
        ++i;
    }
    tmp->list.next = NULL;
    tmp->list.prev = prev;
    return (0);
}

static uint32_t __pmm_get_last_addr(void)
{
    PMM_BLOCK *tmp = NULL;
    __PMM_GET_BLOCK(tmp);

    assert(tmp == NULL);
    while (tmp->list.next != NULL)
        tmp = tmp->list.next;
    return ((uint32_t)tmp);
}

static PMM_BLOCK *__pmm_get_first_free_block(void)
{
    PMM_BLOCK *tmp = NULL;
    __PMM_GET_BLOCK(tmp);

    while (tmp)
    {
        if (tmp->state == PMM_BLOCK_FREE)
            return (tmp);
        tmp = tmp->list.next;
    }
    return (NULL);
}

static PMM_BLOCK *__pmm_get_first_free_block_by_size(uint32_t size)
{
    if (size == 0)
        return (NULL);
    else if (size == 1)
        return (__pmm_get_first_free_block());
    else
    {
        PMM_BLOCK *tmp = NULL;

        __PMM_GET_BLOCK(tmp);
        while (tmp != NULL)
        {
            if (tmp->state == PMM_BLOCK_FREE)
            {
                uint32_t i = 0;
                while (tmp != NULL && i < size)
                {
                    if (tmp->list.next->state != PMM_BLOCK_FREE)
                        break;
                    tmp = tmp->list.next;
                    ++i;
                }
                if (i == size)
                    return (tmp);
            }
            tmp = tmp->list.next;
        }
    }
    return (NULL);
}

static void __pmm_set_block(PMM_BLOCK *block, uint32_t size, enum e_pmm_block_state state)
{
    block->size = size;
    block->state = state;
    block->addr = (block->id * PMM_BLOCK_SIZE) + pmm_info.infos.memory_map_end;
}

static void *__pmm_alloc_block(void)
{
    if (PMM_OUT_OF_MEMORY(pmm_info) == true)
        return (NULL);
    else
    {
        PMM_BLOCK *frame = __pmm_get_first_free_block();

        if (frame == NULL)
            return (NULL);
        else
        {
            __pmm_set_block(frame, 1, PMM_BLOCK_USED);
            ++pmm_info.infos.used_blocks;
            return ((void *)frame->addr);
        }
    }
    return (NULL);
}

static void *__pmm_alloc_blocks(uint32_t size)
{
    if (PMM_OUT_OF_MEMORY(pmm_info) == true)
        return (NULL);
    else
    {
        PMM_BLOCK *frame = __pmm_get_first_free_block_by_size(size);

        if (frame == NULL)
            return (NULL);
        else
        {
            PMM_BLOCK *tmp = frame;
            for (uint32_t i = 0; i <= size; i++)
            {
                __pmm_set_block(tmp, size, PMM_BLOCK_USED);
                tmp = tmp->list.next;
            }
            pmm_info.infos.used_blocks += size;
            return ((void *)frame->addr);
        }
    }
    return (NULL);
}

static void __pmm_free_block(void *addr)
{
    PMM_BLOCK *tmp = NULL;

    __PMM_GET_BLOCK(tmp);
    while (tmp != NULL)
    {
        if (tmp->addr == (pmm_physical_addr_t)addr)
        {
            __pmm_set_block(tmp, 0, PMM_BLOCK_FREE);
            --pmm_info.infos.used_blocks;
            return;
        }
        tmp = tmp->list.next;
    }
}

static void __pmm_free_blocks(void *addr, uint32_t size)
{
    PMM_BLOCK *tmp = NULL;

    __PMM_GET_BLOCK(tmp);
    while (tmp != NULL)
    {
        if (tmp->addr == (pmm_physical_addr_t)addr)
        {
            for (uint32_t i = 0; i <= size; i++)
            {
                __pmm_set_block(tmp, 0, PMM_BLOCK_FREE);
                tmp = tmp->list.next;
            }
            pmm_info.infos.used_blocks -= size;
            return;
        }
        tmp = tmp->list.next;
    }
}

static void __pmm_swap_blocks(PMM_BLOCK *block1, PMM_BLOCK *block2)
{
    PMM_BLOCK tmp = *block1;

    *block1 = *block2;
    *block2 = tmp;
}

/*
** Sort blocks by address (Simple Bubble Sort) (TODO : Improve)
*/
static void __pmm_defragment(void)
{
    PMM_BLOCK *tmp = NULL;

    __PMM_GET_BLOCK(tmp);

    while (tmp != NULL)
    {
        if (tmp->state == PMM_BLOCK_FREE)
        {
            PMM_BLOCK *tmp2 = tmp->list.next;
            while (tmp2 != NULL)
            {
                if (tmp2->state == PMM_BLOCK_FREE)
                {
                    __pmm_swap_blocks(tmp, tmp2);
                    break;
                }
                tmp2 = tmp2->list.next;
            }
        }
        tmp = tmp->list.next;
    }
}

/*******************************************************************************
 *                            GLOBAL PMM FUNCTIONS                             *
 ******************************************************************************/

/* Init PMM */
int pmm_init(pmm_physical_addr_t bitmap, uint32_t total_memory_size)
{
    /* INIT PMM INFO */
    pmm_info.infos.memory_size = total_memory_size;
    pmm_info.infos.max_blocks = PMM_SIZE;
    pmm_info.infos.memory_map_start = bitmap;
    pmm_info.infos.memory_map_length = PMM_END_ADDR - pmm_info.infos.memory_map_start;
    pmm_info.infos.used_blocks = 0;
    pmm_info.blocks = (pmm_block_t *)bitmap;

    kprintf("PMM: Memory Size : %u Mo\n", pmm_info.infos.memory_size / 1024 / 1024);
    kprintf("PMM: Max Blocks : %u\n", pmm_info.infos.max_blocks);
    kprintf("PMM: Memory Map Start : 0x%x\n", pmm_info.infos.memory_map_start);
    kprintf("PMM: End ADDR : 0x%x\n", PMM_END_ADDR);

    /* INIT PMM BLOCKS */
    __pmm_init_blocks(pmm_info.infos.max_blocks);
    pmm_info.infos.memory_map_end = (pmm_physical_addr_t)__pmm_get_last_addr();
    kprintf("PMM: Memory Map End : 0x%x\n", pmm_info.infos.memory_map_end);

    kprintf("PMM: Bitmap 0x%x\n", bitmap);
    kprintf("PMM: blocks 0x%x\n", pmm_info.blocks);
    return (0);
}

/* Alloc */
void *pmm_alloc_block(void)
{
    return (__pmm_alloc_block());
}

void pmm_free_block(void *addr)
{
    __pmm_free_block(addr);
}

void *pmm_alloc_blocks(uint32_t size)
{
    return (__pmm_alloc_blocks(size));
}

void pmm_free_blocks(void *addr, uint32_t size)
{
    __pmm_free_blocks(addr, size);
}

/* Utils */
PMM_BLOCK *pmm_get_next_available_block(void)
{
    return (__pmm_get_first_free_block());
}

PMM_BLOCK *pmm_get_next_available_blocks(uint32_t size)
{
    return (__pmm_get_first_free_block_by_size(size));
}

/* Defragment */
void pmm_defragment(void)
{
    __pmm_defragment();
}

/*******************************************************************************
 *                                  PMM UTILS                                  *
 ******************************************************************************/

uint32_t pmm_get_max_blocks(void)
{
    return (pmm_info.infos.max_blocks);
}

uint32_t pmm_get_memory_size(void)
{
    return (pmm_info.infos.memory_size);
}

uint32_t pmm_get_memory_map_start(void)
{
    return (pmm_info.infos.memory_map_start);
}

uint32_t pmm_get_memory_map_end(void)
{
    return (pmm_info.infos.memory_map_end);
}

uint32_t pmm_get_memory_map_length(void)
{
    return (pmm_info.infos.memory_map_length);
}

uint32_t pmm_get_block_size(void)
{
    return (PMM_BLOCK_SIZE);
}

/*******************************************************************************
 *                                  TEST PMM                                   *
 ******************************************************************************/

void pmm_display_blocks(uint32_t size)
{
    uint32_t i;

    i = 0;
    while (i < size)
    {
        kprintf("Block %d: " COLOR_YELLOW "%s" COLOR_END ", addr: 0x%x\n", i,
                pmm_info.blocks[i].state == PMM_BLOCK_FREE ? "FREE" : "USED",
                pmm_info.blocks[i].addr);
        ++i;
    }
}
