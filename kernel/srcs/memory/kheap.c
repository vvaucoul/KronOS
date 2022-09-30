/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 00:33:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 13:21:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

#include <system/pit.h>

Heap kheap;

/*******************************************************************************
 *                           PRIVATE HEAP FUNCTIONS                            *
 ******************************************************************************/

static void __init_heap(uint32_t new_end)
{
    HeapBlock *block = (HeapBlock *)kheap.root;
}

static void __expand_heap(uint32_t size)
{
    kprintf("Expanding heap by %d bytes\n", size);
    HeapBlock *old_root = kheap.root;
    data_t *new_addr_start = NULL;
    data_t *new_addr_end = NULL;

    kheap.allocated_blocks += 1;
    new_addr_start = PHYSICAL_EXPAND_HEAP_START(PHYSICAL_EXPAND_HEAP_START_SIZE + (PHYSICAL_EXPAND_HEAP_START_OFFSET * kheap.allocated_blocks));
    new_addr_end = PHYSICAL_EXPAND_HEAP_END(kheap.end_addr, size);

    // kheap.start_addr =
    // kheap.end_addr =
    kprintf("New Heap End : %x\n", kheap.end_addr);
}

static HeapBlock *__get_first_free_block(uint32_t size)
{
    HeapBlock *block = kheap.root;

    while (block != NULL)
    {
        kprintf("- Check Block : 0x%x | %d | %d >= %d = %d\n", block, block->metadata.state, block->metadata.size, size, block->metadata.size >= size);
        if (block->metadata.state == HEAP_BLOCK_FREE && block->metadata.size >= size)
            return (block);
        else
            block = block->next;
    }
    kprintf("No free block found\n");
    return (NULL);
}

static int __init(data_t *start_addr, data_t *end_addr)
{
    if (start_addr == NULL || end_addr == NULL)
        return (1);
    else if (start_addr >= end_addr)
        return (1);
    else
    {
        kheap.start_addr = start_addr;
        kheap.end_addr = end_addr;
        kheap.max_size = end_addr - start_addr;
        kheap.used_size = 0;
        kheap.allocated_blocks = PHYSICAL_MEMORY_BLOCKS;
        kheap.root = NULL;
    }
    return (0);
}

static uint32_t __ksize(data_t *ptr)
{
    HeapBlock *tmp = kheap.root;

    while (tmp != NULL)
    {
        if (tmp->data == ptr)
            return (tmp->metadata.size);
        tmp = tmp->next;
    }
    return (0);
}

static data_t *__kbrk(uint32_t size)
{
    data_t *addr = NULL;

    kprintf("Check Physical End: %u + %u = %u >= %u ? = %d\n", kheap.used_size, size, kheap.used_size + size, PHYSICAL_LENGTH, kheap.used_size + size >= PHYSICAL_END);
    if (kheap.used_size + size >= PHYSICAL_END)
    {
        kprintf("Not enough memory to allocate %d bytes !\n", size);
        return (NULL);
    }
    kprintf("Check Expand Heap: %u + %u = %u >= %u ? = %d\n", kheap.used_size, size, kheap.used_size + size, kheap.max_size, kheap.used_size + size >= kheap.max_size);
    if (kheap.used_size + size >= kheap.max_size)
        __expand_heap(PHYSICAL_EXPAND_HEAP_SIZE);
    addr = (kheap.start_addr + kheap.used_size + size + SIZEOF_KBRK());
    kheap.used_size += size + SIZEOF_KBRK();
    return (addr);
}

static HeapBlock *__allocate_new_block(uint32_t size)
{
    HeapBlock *tmp = kheap.root;

    if (tmp != NULL)
    {
        while (tmp->next != NULL)
            tmp = tmp->next;
    }

    HeapBlock *new_block = (HeapBlock *)__kbrk(sizeof(HeapBlock));
    if (new_block == NULL)
        return (NULL);
    else
    {
        new_block->metadata.state = HEAP_BLOCK_USED;
        new_block->metadata.size = size;
        new_block->data = (data_t *)__kbrk(size);
        new_block->next = NULL;
        if (new_block->data == NULL)
            return (NULL);
        tmp->next = new_block;
        return (new_block);
    }
}

static data_t *__kmalloc(uint32_t size)
{
    if (size <= 0)
        return (NULL);
    else
    {
        kprintf("Kmalloc %u bytes\n", size);
        if (kheap.root == NULL)
        {
            kprintf("- Init Heap\n");
            kheap.root = (HeapBlock *)__kbrk(sizeof(HeapBlock));
            kheap.root->metadata.size = size;
            kheap.root->metadata.state = HEAP_BLOCK_USED;
            kheap.root->next = NULL;
            kheap.root->data = (data_t *)__kbrk(size);
            return (kheap.root->data);
        }
        else
        {
            kprintf("- Get First Free Block\n");
            HeapBlock *block = __get_first_free_block(size);
            kprintf("- Block: 0x%x\n", block);

            if (block == NULL)
            {
                kprintf("- Allocate New Block : %u bytes\n", size);
                HeapBlock *new_block = __allocate_new_block(size);
                kprintf("- New Block: 0x%x\n", new_block);
                if (new_block == NULL)
                    return (NULL);
                else
                {
                    new_block->metadata.state = HEAP_BLOCK_USED;
                    new_block->metadata.size = size;
                    new_block->data = (data_t *)__kbrk(size);
                    return (new_block->data);
                }
            }
            else
            {
                block->metadata.state = HEAP_BLOCK_USED;
                return (block->data);
            }
        }
    }
}

static data_t *__kcalloc(uint32_t count, uint32_t size)
{
    if (count < 0 || size < 0)
        return (NULL);
    else
    {
        data_t *addr = __kmalloc(count * size);
        if (addr == NULL)
            return (NULL);
        kmemset(addr, 0, count * size);
        return (addr);
    }
}

static data_t *__krealloc(data_t *addr, uint32_t size)
{
    HeapBlock *block = kheap.root;

    if (addr == NULL)
        return (NULL);
    else if (block == NULL)
        return (NULL);
    else
    {
        while (block != NULL)
        {
            if (block->data == addr)
            {
                if (block->metadata.size >= size)
                    return (addr);
                else
                {
                    data_t *new_addr = __kbrk(size);
                    if (new_addr == NULL)
                        return (NULL);
                    else
                    {
                        kmemcpy(new_addr, addr, block->metadata.size);
                        block->data = new_addr;
                        block->metadata.size = size;
                        return (new_addr);
                    }
                }
            }
            block = block->next;
        }
    }
    return (NULL);
}

static void __kfree(data_t *addr)
{
    HeapBlock *block = kheap.root;

    if (addr == NULL)
        return;
    else
    {
        while (block != NULL)
        {
            if (block->data == addr)
            {
                block->metadata.state = HEAP_BLOCK_FREE;
                return;
            }
            block = block->next;
        }
    }
}

/*******************************************************************************
 *                            GLOBAL HEAP FUNCTIONS                            *
 ******************************************************************************/

int kheap_init(data_t *start_addr, data_t *end_addr)
{
    return (__init(start_addr, end_addr));
}

data_t *kmalloc(uint32_t size)
{
    return (__kmalloc(size));
}

data_t *krealloc(void *ptr, uint32_t size)
{
    return (__krealloc(ptr, size));
}

void kfree(void *ptr)
{
    __kfree(ptr);
}

uint32_t ksize(data_t *ptr)
{
    return (__ksize(ptr));
}

data_t *kcalloc(uint32_t count, uint32_t size)
{
    return (__kcalloc(count, size));
}

data_t *kbrk(uint32_t size)
{
    return (__kbrk(size));
}

data_t *kmmap(uint32_t size)
{
    __UNUSED(size);
    return (NULL);
}