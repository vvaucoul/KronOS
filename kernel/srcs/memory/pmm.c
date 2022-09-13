/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 12:06:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 20:57:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/pmm.h>

PMM_INFO pmm_info;

/*******************************************************************************
 *                            PRIVATE PMM FUNCTIONS                            *
 ******************************************************************************/

static int __pmm_init_blocks(uint32_t max_size)
{
    uint32_t i;

    i = 0;
    while (i < max_size)
    {
        pmm_info.blocks[i] = PMM_INIT_BLOCK();
        ++i;
    }
    return (0);
}

static int __pmm_get_first_free_block(void)
{
    uint32_t i;

    i = 0;
    while (i < pmm_info.infos.max_blocks)
    {
        if (pmm_info.blocks[i].state == PMM_BLOCK_FREE)
            return (i);
        ++i;
    }
    return (PMM_ERR_NO_FREE_BLOCK);
}

static int __pmm_get_first_free_block_by_size(uint32_t size)
{
    if (size == 0)
        return (PMM_ERR_NO_FREE_BLOCK);
    else if (size == 1)
        return (__pmm_get_first_free_block());
    else
    {
        for (uint32_t i = 0; i < pmm_info.infos.max_blocks; ++i)
        {
            bool is_valid = true;
            if (pmm_info.blocks[i].state != PMM_BLOCK_FREE)
                continue;
            for (uint32_t j = i; (i <= size) && (i + j < pmm_info.infos.max_blocks); j++)
            {
                if (pmm_info.blocks[j].state != PMM_BLOCK_FREE)
                {
                    is_valid = false;
                    break;
                }
            }
            if (is_valid)
                return (i);
        }
    }
    return (PMM_ERR_NO_FREE_BLOCK);
}

static void __pmm_set_block(uint32_t block_id, uint32_t size, enum e_pmm_block_state state)
{
    pmm_info.blocks[block_id].size = size;
    pmm_info.blocks[block_id].state = state;
    pmm_info.blocks[block_id].addr = (block_id * PMM_BLOCK_SIZE) + pmm_info.infos.memory_map_end;
}

static void *__pmm_alloc_block(void)
{
    if (PMM_OUT_OF_MEMORY(pmm_info) == true)
        return (NULL);
    else
    {
        int frame = __pmm_get_first_free_block();

        if (frame == PMM_ERR_NO_FREE_BLOCK)
            return (NULL);
        else
        {
            __pmm_set_block(frame, 1, PMM_BLOCK_USED);
            ++pmm_info.infos.used_blocks;
            return ((void *)pmm_info.blocks[frame].addr);
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
        int frame = __pmm_get_first_free_block_by_size(size);

        if (frame == PMM_ERR_NO_FREE_BLOCK)
            return (NULL);
        else
        {
            for (uint32_t i = 0; i <= size; i++)
                __pmm_set_block(frame + i, 1, PMM_BLOCK_USED);
            pmm_info.infos.used_blocks += size;
            return ((void *)pmm_info.blocks[frame].addr);
        }
    }
    return (NULL);
}

static void __pmm_free_block(void *addr)
{
    uint32_t i;

    i = 0;
    while (i < pmm_info.infos.max_blocks)
    {
        if (pmm_info.blocks[i].addr == (pmm_physical_addr_t)addr)
        {
            __pmm_set_block(i, 0, PMM_BLOCK_FREE);
            --pmm_info.infos.used_blocks;
            return;
        }
        ++i;
    }
}

static void __pmm_free_blocks(void *addr, uint32_t size)
{
    uint32_t i;

    i = 0;
    while (i < pmm_info.infos.max_blocks)
    {
        if (pmm_info.blocks[i].addr == (pmm_physical_addr_t)addr)
        {
            for (uint32_t j = 0; (j <= size) && (i + j < pmm_info.infos.max_blocks); j++)
                __pmm_set_block(i + j, 0, PMM_BLOCK_FREE);
            pmm_info.infos.used_blocks -= size;
            return;
        }
        ++i;
    }
}

static void __pmm_swap_blocks(uint32_t block1, uint32_t block2)
{
    PMM_BLOCK tmp = pmm_info.blocks[block1];
    pmm_info.blocks[block1] = pmm_info.blocks[block2];
    pmm_info.blocks[block2] = tmp;
}

/*
** Sort blocks by address (Simple Bubble Sort) (TODO : Improve)
*/
static void __pmm_defragment(void)
{
    uint32_t i;

    i = 0;
    while (i < pmm_info.infos.max_blocks)
    {
        if (pmm_info.blocks[i].state == PMM_BLOCK_FREE && pmm_info.blocks[i + 1].state != PMM_BLOCK_FREE)
        {
            __pmm_swap_blocks(i, i + 1);
            i = 0;
            continue;
        }
        else
            ++i;
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
    pmm_info.infos.max_blocks = total_memory_size / PMM_BLOCK_SIZE;
    pmm_info.infos.memory_map_start = bitmap;
    pmm_info.infos.memory_map_length = pmm_info.infos.memory_map_end - pmm_info.infos.memory_map_start;

    /* INIT PMM BLOCKS */
    pmm_info.blocks = (pmm_block_t *)bitmap;
    __pmm_init_blocks(pmm_info.infos.max_blocks * sizeof(pmm_physical_addr_t));

    pmm_info.infos.used_blocks = 0x0;
    pmm_info.infos.memory_map_end = (uint32_t)(&(pmm_info.blocks[pmm_info.infos.max_blocks]));

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
int pmm_get_next_available_block(void)
{
    return (__pmm_get_first_free_block());
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

void pmm_display(void)
{
    kprintf("PMM INFO:\n");
    kprintf("- " COLOR_YELLOW "Memory Size" COLOR_END ": " COLOR_GREEN "%u" COLOR_END " | " COLOR_GREEN "0x%x\n" COLOR_END, pmm_info.infos.memory_size, pmm_info.infos.memory_size);
    kprintf("- " COLOR_YELLOW "Max Blocks" COLOR_END ": " COLOR_GREEN "%u\n" COLOR_END, pmm_info.infos.max_blocks);
    kprintf("- " COLOR_YELLOW "Memory Map Start" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, pmm_info.infos.memory_map_start);
    kprintf("- " COLOR_YELLOW "Memory Map End" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, pmm_info.infos.memory_map_end);
    kprintf("- " COLOR_YELLOW "Memory Map Length" COLOR_END ": " COLOR_GREEN "%u\n" COLOR_END, pmm_info.infos.memory_map_length);
    kprintf("- " COLOR_YELLOW "Blocks" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, pmm_info.blocks);
}

int pmm_test(void)
{
    kprintf("PMM Test: \n");
    kprintf("[Kernel Region " COLOR_GREEN "0" COLOR_END "-" COLOR_GREEN "%u" COLOR_END "] - [" COLOR_GREEN "0x%x" COLOR_END "-" COLOR_GREEN "0x%x" COLOR_END "]\n", pmm_get_max_blocks(), pmm_get_memory_map_start(), pmm_get_memory_map_end());
    uint32_t *ptr1 = pmm_alloc_block();
    if (ptr1 == NULL)
        kprintf("BLock 0 is NULL\n");
    else
        kprintf("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr1, pmm_get_next_available_block());
    uint32_t *ptr2 = pmm_alloc_blocks(5);
    if (ptr2 == NULL)
        kprintf("BLock 1-5 is NULL\n");
    else
        kprintf("Alloc 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr2, pmm_get_next_available_block());
    uint32_t *ptr3 = pmm_alloc_block();
    if (ptr3 == NULL)
        kprintf("BLock 6 is NULL\n");
    else
        kprintf("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr3, pmm_get_next_available_block());
    uint32_t *ptr4 = pmm_alloc_blocks(5);
    if (ptr4 == NULL)
        kprintf("BLock 7-11 is NULL\n");
    else
        kprintf("Alloc 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr4, pmm_get_next_available_block());
    uint32_t *ptr5 = pmm_alloc_block();
    if (ptr5 == NULL)
        kprintf("BLock 12 is NULL\n");
    else
        kprintf("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr5, pmm_get_next_available_block());

    kprintf("\n");

    kbzero(ptr1, PMM_BLOCK_SIZE);
    kmemcpy(ptr1, "Hello World", 12);
    kprintf("ptr1: %s at 0x%x \n", ptr1, ptr1);
    ptr2[0] = 0x12345678;
    ptr2[1] = 0x87654321;
    ptr2[2] = 0x12345678;
    ptr2[3] = 0x87654321;
    ptr2[4] = 0x12345678;
    kprintf("ptr2: %x %x %x %x %x at 0x%x \n", ptr2[0], ptr2[1], ptr2[2], ptr2[3], ptr2[4], ptr2);

    ptr3 = kmemcpy(ptr3, "42Born2Code", 12);
    kprintf("ptr3: %s at 0x%x \n", ptr3, ptr3);
    ptr4 = kmemcpy(ptr4, "Kernel", 7);
    kprintf("ptr4: %s at 0x%x \n", ptr4, ptr4);
    ptr5 = kmemcpy(ptr5, "Test123", 8);
    kprintf("ptr5: %s at 0x%x \n", ptr5, ptr5);

    kprintf("\n");

    pmm_free_block(ptr5);
    kprintf("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr5, pmm_get_next_available_block());
    pmm_free_blocks(ptr4, 5);
    kprintf("Free 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr4, pmm_get_next_available_block());
    pmm_free_block(ptr3);
    kprintf("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr3, pmm_get_next_available_block());
    pmm_free_blocks(ptr2, 5);
    kprintf("Free 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr2, pmm_get_next_available_block());
    pmm_free_block(ptr1);
    kprintf("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr1, pmm_get_next_available_block());
    kprintf("\nNext Available block " COLOR_GREEN "%d" COLOR_END " (should be 0)\n", pmm_get_next_available_block());
    return (0);
}

int pmm_defragment_test(void)
{
    kprintf("PMM Defragment Test: \n");
    uint32_t *ptr1 = pmm_alloc_block();
    kprintf("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr1, pmm_get_next_available_block());
    uint32_t *ptr2 = pmm_alloc_block();
    kprintf("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr2, pmm_get_next_available_block());
    uint32_t *ptr3 = pmm_alloc_block();
    kprintf("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr3, pmm_get_next_available_block());

    kprintf("\n");
    pmm_display_blocks(3);
    kprintf("\n");
    pmm_free_block(ptr2);
    kprintf("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr2, pmm_get_next_available_block());
    pmm_display_blocks(3);
    kprintf("\n");
    pmm_defragment();
    kprintf("Defragmentation done\n");
    pmm_display_blocks(3);
    kprintf("\n");
    pmm_free_block(ptr1);
    kprintf("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr1, pmm_get_next_available_block());
    pmm_free_block(ptr3);
    kprintf("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "%d" COLOR_END "\n", ptr3, pmm_get_next_available_block());
    pmm_display_blocks(3);
    kprintf("\n");
}