/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 01:07:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>
#include <memory/paging.h>

uint32_t placement_addr = (uint32_t)(&__kernel_section_end);
heap_t *kheap = NULL;

static void *__kbrk(uint32_t size, uint8_t align)
{
    return (0);
}

static uint32_t __kmalloc_int(uint32_t size, int align, uint32_t *phys)
{
    /* If Heap exists -> Virtual Memory */
    if (kheap)
    {
        void *addr = __kbrk(size, (uint8_t)align);
        if (phys)
        {
            page_t *page = get_page((uint32_t)addr, kernel_directory);
            *phys = page->frame * 0x1000 + ((uint32_t)addr & 0xFFF);
        }
        return (uint32_t)addr;
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
        uint32_t tmp = placement_addr;
        placement_addr += size;
        return (tmp);
    }
}

static uint32_t __krealloc(void *ptr, uint32_t size)
{
    return (0);
}

static uint32_t __kcalloc(void *ptr, uint32_t size)
{
    return (0);
}

static void __kfree(void *ptr)
{
    if (kheap)
    {
        // TODO
    }
}

static heap_t *__init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly)
{
    kprintf("KHEAP : Initializing kernel heap\n");
    heap_t *heap = (heap_t *)__kmalloc_int(sizeof(heap_t), 0, 0);
    kprintf("Heap : 0x%x\n", heap);
    heap->start_address = start_addr;
    kpause();
    heap->end_address = end_addr;
    heap->max_address = max_addr;
    heap->root = NULL;
    return (heap);
}

/*******************************************************************************
 *                             INTERFACE FUNCTIONS                             *
 ******************************************************************************/

void init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly)
{
    kheap = __init_heap(start_addr, end_addr, max_addr, supervisor, readonly);
}

uint32_t kmalloc_int(uint32_t size, int align, uint32_t *phys)
{
    return (__kmalloc_int(size, align, phys));
}

uint32_t kmalloc_a(uint32_t size)
{
    return (__kmalloc_int(size, 1, 0));
}

uint32_t kmalloc_p(uint32_t size, uint32_t *phys)
{
    return (__kmalloc_int(size, 0, phys));
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *phys)
{
    return (__kmalloc_int(size, 1, phys));
}

uint32_t kmalloc(uint32_t size)
{
    return (__kmalloc_int(size, 0, 0));
}

void kfree(void *ptr)
{
    __kfree(ptr);
}

uint32_t krealloc(void *ptr, uint32_t size)
{
    return (__krealloc(ptr, size));
}

uint32_t kcalloc(void *ptr, uint32_t size)
{
    return (__kcalloc(ptr, size));
}

void *kbrk(uint32_t size)
{
    return (__kbrk(size, 0));
}
