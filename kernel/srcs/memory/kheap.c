/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 00:33:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/27 12:29:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

Heap kheap;

/*******************************************************************************
 *                           PRIVATE HEAP FUNCTIONS                            *
 ******************************************************************************/

static void __expand_heap(uint32_t size)
{
    kprintf("\nExpand Heap !\n");
    // uint32_t old_size = kheap.size;
    // uint32_t new_size = old_size + size;
    // uint32_t new_end = kheap.start + new_size;
    
    kheap.end_addr = PHYSICAL_EXPAND_HEAP(kheap.end_addr, size);
}

static HeapBlock *__get_first_free_block(uint32_t size)
{
    HeapBlock *block = kheap.root;

    while (block)
    {
        if (block->metadata.state == HEAP_BLOCK_FREE && block->metadata.size >= size)
            return (block);
        block = block->next;
    }
    return (NULL);
}

static int __init(void *start_addr, void *end_addr)
{
    if (!start_addr || !end_addr)
        return (1);
    else if (start_addr >= end_addr)
        return (1);
    else
    {
        kheap.start_addr = start_addr;
        kheap.end_addr = end_addr;
        kheap.max_size = end_addr - start_addr;
        kheap.used_size = 0;
        kheap.root = NULL;
    }
    return (0);
}

static uint32_t __ksize(data_t *ptr)
{
    HeapBlock *tmp = kheap.root;

    while (tmp)
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

    if (kheap.used_size + size > PHYSICAL_END)
        return (NULL);
    if (kheap.used_size + size > kheap.max_size)
        __expand_heap(PHYSICAL_EXPAND_HEAP_SIZE);
    addr = (kheap.start_addr + kheap.used_size + size + SIZEOF_KBRK());
    kheap.used_size += size + SIZEOF_KBRK();
    return (addr);
}

static HeapBlock *__allocate_new_block(uint32_t size)
{
    HeapBlock *tmp = kheap.root;

    while (tmp->next)
        tmp = tmp->next;

    HeapBlock *new_block = (HeapBlock *)__kbrk(sizeof(HeapBlock));
    if (!new_block)
        return (NULL);
    else
    {
        new_block->metadata.state = HEAP_BLOCK_USED;
        new_block->metadata.size = size;
        new_block->data = (data_t *)__kbrk(size);
        new_block->next = NULL;
        if (!new_block->data)
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
        if (kheap.root == NULL)
        {
            kheap.root = (HeapBlock *)__kbrk(sizeof(HeapBlock));
            kheap.root->metadata.size = size;
            kheap.root->metadata.state = HEAP_BLOCK_USED;
            kheap.root->next = NULL;
            kheap.root->data = (data_t *)__kbrk(size);
            return (kheap.root->data);
        }
        else
        {
            HeapBlock *block = __get_first_free_block(size);

            if (block == NULL)
            {
                HeapBlock *new_block = __allocate_new_block(size);
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
        if (!addr)
            return (NULL);
        kmemset(addr, 0, count * size);
        return (addr);
    }
}

static data_t *__krealloc(data_t *addr, uint32_t size)
{
    HeapBlock *block = kheap.root;

    if (!addr)
        return (NULL);
    else if (!block)
        return (NULL);
    else
    {
        while (block)
        {
            if (block->data == addr)
            {
                if (block->metadata.size >= size)
                    return (addr);
                else
                {
                    data_t *new_addr = __kbrk(size);
                    if (!new_addr)
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

    if (!addr)
        return;
    else
    {
        while (block)
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

int kheap_init(void *start_addr, void *end_addr)
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