/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/21 19:27:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>
#include <memory/paging.h>

#include <system/panic.h>

uint32_t placement_addr = (uint32_t)(&__kernel_section_end);
heap_t *kheap = NULL;

static void *__kbrk(uint32_t size)
{
    void *addr = NULL;

    if (size == 0)
        return (NULL);
    else
    {
        addr = (void *)placement_addr;
        placement_addr += size;
    }
    return (addr);
}

static void *__kmalloc_int(uint32_t size, bool align, uint32_t *phys)
{
    /* If Heap exists -> Heap Algorithm with Virtual Memory */
    if (kheap)
    {
        // void *addr = kheap_tree_alloc_memory(size);
        void *addr = kheap_alloc(size, align, kheap);
        if (phys)
        {
            page_t *page = get_page((uint32_t)addr, kernel_directory);
            *phys = page->frame * PAGE_SIZE + ((uint32_t)addr & 0xFFF);
        }
        return (addr);
    }
    /* Either, use physical Memory */
    else
    {
        if (align && (placement_addr & 0xFFFFF000))
        {
            placement_addr &= 0xFFFFF000;
            placement_addr += 0x1000;
        }
        if (phys)
        {
            *phys = placement_addr;
        }
        /* Don't need to store data in block -> reverved for system */
        return (__kbrk(size));
    }
}

static uint32_t __ksize(void *ptr)
{
    return (kheap_get_ptr_size(ptr, kheap));
}

static void *__krealloc(void *ptr, uint32_t size)
{
    return (0);
}

static void *__kcalloc(void *ptr, uint32_t size)
{
    return (0);
}

static void __kfree(void *ptr)
{
    kheap_free(ptr, kheap);
}

static uint8_t header_t_less_than(void *a, void *b)
{
    return (((heap_header_t *)a)->size < ((heap_header_t *)b)->size) ? 1 : 0;
}

static heap_t *__init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly)
{
    if (!IS_ALIGNED(start_addr) || !IS_ALIGNED(end_addr))
        __PANIC("KHEAP : Start and End address must be aligned to 0x1000");

    assert(start_addr % 0x1000 == 0);
    assert(end_addr % 0x1000 == 0);

    heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

    heap->array = heap_array_create((void *)start_addr, HEAP_INDEX_SIZE, &header_t_less_than);

    start_addr += sizeof(type_t) * HEAP_INDEX_SIZE;

    if (start_addr & 0xFFFFF000)
    {
        start_addr &= 0xFFFFF000;
        start_addr += 0x1000;
    }

    heap->addr.start_address = start_addr;
    heap->addr.end_address = end_addr;
    heap->addr.max_address = max_addr;
    heap->flags.supervisor = supervisor;
    heap->flags.readonly = readonly;

    heap_header_t *hole = (heap_header_t *)start_addr;
    hole->size = end_addr - start_addr;
    hole->magic = KHEAP_MAGIC;
    hole->state = FREE;
    heap_array_insert_element((void *)hole, &heap->array);

    return (heap);
}

/*******************************************************************************
 *                             INTERFACE FUNCTIONS                             *
 ******************************************************************************/

void init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly)
{
    kheap = __init_heap(start_addr, end_addr, max_addr, supervisor, readonly);
    assert(kheap != NULL);
}

void *kmalloc_int(uint32_t size, bool align, uint32_t *phys)
{
    return (__kmalloc_int(size, align, phys));
}

void *kmalloc_a(uint32_t size)
{
    return (__kmalloc_int(size, 1, 0));
}

void *kmalloc_p(uint32_t size, uint32_t *phys)
{
    return (__kmalloc_int(size, 0, phys));
}

void *kmalloc_ap(uint32_t size, uint32_t *phys)
{
    return (__kmalloc_int(size, 1, phys));
}

void *kmalloc(uint32_t size)
{
    return (__kmalloc_int(size, 0, 0));
}

void kfree(void *ptr)
{
    // Todo : Free memory
    // __kfree(ptr);
}

void *krealloc(void *ptr, uint32_t size)
{
    return (__krealloc(ptr, size));
}

void *kcalloc(void *ptr, uint32_t size)
{
    return (__kcalloc(ptr, size));
}

void *kbrk(uint32_t size)
{
    return (__kbrk(size));
}

uint32_t ksize(void *ptr)
{
    return (__ksize(ptr));
}