/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 00:33:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/18 16:33:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

#include <system/pit.h>

Heap kheap;

/*******************************************************************************
 *                           PRIVATE HEAP FUNCTIONS                            *
 ******************************************************************************/

static int __expand_heap(uint32_t size)
{
    // kprintf(COLOR_YELLOW "Expand BLOCK\n" COLOR_END);
    void *new_block = pmm_alloc_blocks(PHYSICAL_MEMORY_BLOCKS);
    if (new_block == NULL)
        return (1);
    void *new_end_addr = (void *)(kheap.end_addr + (pmm_get_next_available_block() * (size)));
    kheap.end_addr = new_end_addr;
    kheap.max_size = kheap.end_addr - kheap.start_addr;
    // kprintf(COLOR_GREEN "New Max Size: %u Ko\n" COLOR_END, kheap.max_size / 1024);
    // timer_wait(1000);
    return (0);
}

static HeapBlock *__get_first_free_block(uint32_t size)
{
    HeapBlock *block = kheap.root;

    while (block != NULL)
    {
        // kprintf("- Check Block : 0x%x | State: %d | B-Size: %d >= Size: %d = Valid: %d\n", block, block->metadata.state, block->metadata.size, size, block->metadata.state == HEAP_BLOCK_FREE && block->metadata.size >= size);

        if (block->metadata.state == HEAP_BLOCK_FREE && block->metadata.size >= size)
            return (block);
        else
            block = block->next;
    }
    // kprintf("No free block found\n");
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
        // kprintf("Start Addr : 0x%x\n", kheap.start_addr);
        kheap.end_addr = end_addr;
        // kprintf("End Addr : 0x%x\n", kheap.end_addr);
        kheap.max_size = (uint32_t)end_addr - (uint32_t)start_addr;
        // kprintf("Max Size : %u\n", kheap.max_size);
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

    if (kheap.used_size + size >= PHYSICAL_LENGTH)
    {
        kprintf("Not enough memory to allocate %d bytes !\n", size);
        kprintf("Used Size: %d | Size: %d | Max Size: %d\n", kheap.used_size, size, kheap.max_size);
        return (NULL);
    }
    else if (kheap.used_size + size >= kheap.max_size)
    {
        if (__expand_heap(PHYSICAL_EXPAND_HEAP_SIZE) == 1)
        {
            kprintf("Not enough memory to expand heap !\n");
            return (NULL);
        }
    }
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
        new_block->data = __kbrk(size);
        new_block->next = NULL;
        if (new_block->data == NULL)
            return (NULL);
        tmp->next = new_block;
        return (new_block);
    }
}

// static data_t *__kmalloc_int(uint32_t size, int align, uint32_t *phys)
// {
//     if (size == 0)
//         return (NULL);
//     else
//     {
//         if (align == 1 && (__placement_address & 0xFFFFF000))
//         {
//             __placement_address &= 0xFFFFF000;
//             __placement_address += 0x1000;
//         }
//         if (phys)
//             *phys = __placement_address;
//         data_t *tmp = __placement_address;
//         // data_t *tmp = __kmalloc_kernel_heap(size);
//         __placement_address += size;
//         return (tmp);
//     }
//     return (NULL);
// }

static data_t *__init_heap(uint32_t size)
{
    kheap.root = (HeapBlock *)__kbrk(sizeof(HeapBlock));
    if (kheap.root == NULL)
        return (NULL);
    else
    {
        kheap.root->metadata.state = HEAP_BLOCK_USED;
        kheap.root->metadata.size = size;
        kheap.root->data = __kbrk(size);
        kheap.root->next = NULL;
        return (kheap.root->data);
    }
}

static data_t *__kmalloc_kernel_heap(uint32_t size, bool align)
{
    if (size == 0)
        return (NULL);
    else
    {
        if (align == true && ((uint32_t)kheap.last_addr & 0xFFFFF000))
        {
            kheap.last_addr &= 0xFFFFF000;
            kheap.last_addr += 0x1000;
        }
        if (kheap.root == NULL)
        {
            data_t *addr = __init_heap(size);
            kheap.last_addr = (uint32_t)addr;
            return (addr);
        }
        else
        {
            HeapBlock *block = __get_first_free_block(size);
            if (block == NULL)
            {
                block = __allocate_new_block(size);
                if (block == NULL)
                    return (NULL);
                else
                {
                    block->metadata.state = HEAP_BLOCK_USED;
                    block->metadata.size = size;
                    block->data = __kbrk(size);
                    if (block->data == NULL)
                        return (NULL);
                    else
                        kheap.last_addr = (uint32_t)block->data;
                    return (block->data);
                }
            }
            else
            {
                block->metadata.state = HEAP_BLOCK_USED;
                block->metadata.size = size;
                kheap.last_addr = (uint32_t)block->data;
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
        // data_t *addr = __kmalloc(count * size);
        data_t *addr = __kmalloc_kernel_heap(count * size, false);
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
                block->metadata.size = 0;
                block->data = NULL;
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
    return (__kmalloc_kernel_heap(size, false));
}

data_t *kmalloc_int(uint32_t size, int align, uint32_t *phys)
{
    data_t *data = __kmalloc_kernel_heap(size, align);
    *phys = KHEAP_GET_PHYSICAL_ADDR();
    return (data);
}

data_t *kmalloc_a(uint32_t size)
{
    return (__kmalloc_kernel_heap(size, true));
}

data_t *kmalloc_p(uint32_t size, uint32_t *phys)
{
    data_t *data = __kmalloc_kernel_heap(size, false);
    *phys = KHEAP_GET_PHYSICAL_ADDR();
    return (data);
}

data_t *kmalloc_ap(uint32_t size, uint32_t *phys)
{
    data_t *data = __kmalloc_kernel_heap(size, true);
    *phys = KHEAP_GET_PHYSICAL_ADDR();
    return (data);
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