/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:09:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 17:55:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>
#include <memory/paging.h>
#include <memory/frames.h>

static int64_t __kheap_find_hole(uint32_t size, bool align, heap_t *heap)
{
    int64_t index = 0;

    while ((uint32_t)index < heap->array.size)
    {
        heap_header_t *header = (heap_header_t *)heap_array_get_element(index, &heap->array);

        if (align == true)
        {
            uint32_t location = (uint32_t)header;
            int32_t offset = 0;

            if (((location + sizeof(heap_header_t)) & 0xFFFFF000) != 0)
                offset = PAGE_SIZE - (location + sizeof(heap_header_t)) % PAGE_SIZE;

            uint32_t hole_size = (header->size - offset);

            if (hole_size >= size)
                break;
        }
        else if (header->size >= size)
            break;
        index++;
    }

    if ((uint32_t)index == heap->array.size)
        return (-1);
    else
        return (index);
}

static void __kheap_expand_heap(uint32_t new_size, heap_t *heap)
{
    assert(new_size > heap->addr.end_address - heap->addr.start_address);

    if ((new_size & 0xFFFFF000) != 0)
    {
        new_size &= 0xFFFFF000;
        new_size += PAGE_SIZE;
    }

    assert(heap->addr.start_address + new_size <= heap->addr.max_address);

    uint32_t old_size = heap->addr.end_address - heap->addr.start_address;
    uint32_t i = old_size;

    while (i < new_size)
    {
        const page_t *page = get_page(heap->addr.start_address + i, kernel_directory);
        if (!page)
            page = create_page(heap->addr.start_address + i, kernel_directory);
        alloc_frame((page_t *)page, (heap->flags.supervisor) ? 1 : 0, (heap->flags.readonly) ? 0 : 1);
        i += PAGE_SIZE;
    }
    heap->addr.end_address = heap->addr.start_address + new_size;
}

static uint32_t __kheap_contract_heap(uint32_t new_size, heap_t *heap)
{
    assert(new_size < heap->addr.end_address - heap->addr.start_address);

    if ((new_size & PAGE_SIZE) != 0)
    {
        new_size &= PAGE_SIZE;
        new_size += PAGE_SIZE;
    }

    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;

    uint32_t old_size = heap->addr.end_address - heap->addr.start_address;
    uint32_t i = old_size - PAGE_SIZE;

    while (new_size < i)
    {
        const page_t *page = get_page(heap->addr.start_address + i, kernel_directory);
        assert(page != NULL);

        free_frame((page_t *)page);
        i -= PAGE_SIZE;
    }
    heap->addr.end_address = heap->addr.start_address + new_size;
    return (new_size);
}

data_t kheap_alloc(uint32_t size, bool align, heap_t *heap)
{
    uint32_t new_size = size + sizeof(heap_header_t) + sizeof(heap_footer_t);
    int32_t iterator = __kheap_find_hole(new_size, align, heap);

    if (iterator == -1)
    {
        uint32_t old_length = heap->addr.end_address - heap->addr.start_address;
        uint32_t old_end_address = heap->addr.end_address;

        __kheap_expand_heap(old_length + new_size, heap);
        uint32_t new_length = heap->addr.end_address - heap->addr.start_address;

        iterator = 0;

        uint32_t idx = -1;
        uint32_t value = 0x0;
        while ((uint32_t)iterator < heap->array.size)
        {
            uint32_t tmp = (uint32_t)heap_array_get_element(iterator, &heap->array);
            if (tmp > value)
            {
                value = tmp;
                idx = iterator;
            }
            iterator++;
        }

        if (idx == 0xFFFFFFFF)
        {
            heap_header_t *header = (heap_header_t *)old_end_address;
            header->magic = KHEAP_MAGIC;
            header->size = new_length - old_length;
            header->state = FREE;
            heap_footer_t *footer = (heap_footer_t *)(old_end_address + header->size - sizeof(heap_footer_t));
            footer->magic = KHEAP_MAGIC;
            footer->header = header;
            heap_array_insert_element((void *)header, &heap->array);
        }
        else
        {
            heap_header_t *header = heap_array_get_element(idx, &heap->array);
            header->size += new_length - old_length;

            heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));
            footer->header = header;
            footer->magic = KHEAP_MAGIC;
        }

        return kheap_alloc(size, align, heap);
    }

    heap_header_t *orig_hole_header = (heap_header_t *)heap_array_get_element(iterator, &heap->array);
    uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
    uint32_t orig_hole_size = orig_hole_header->size;

    if (orig_hole_size - new_size < sizeof(heap_header_t) + sizeof(heap_footer_t))
    {
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }

    if (align && orig_hole_pos & 0xFFFFF000)
    {
        uint32_t new_location = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - sizeof(heap_header_t);
        heap_header_t *hole_header = (heap_header_t *)orig_hole_pos;
        hole_header->size = 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - sizeof(heap_header_t);
        hole_header->magic = KHEAP_MAGIC;
        hole_header->state = FREE;
        heap_footer_t *hole_footer = (heap_footer_t *)((uint32_t)new_location - sizeof(heap_footer_t));
        hole_footer->magic = KHEAP_MAGIC;
        hole_footer->header = hole_header;
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header->size;
    }
    else
    {
        heap_array_remove_element(iterator, &heap->array);
    }

    heap_header_t *block_header = (heap_header_t *)orig_hole_pos;
    block_header->magic = KHEAP_MAGIC;
    block_header->state = USED;
    block_header->size = new_size;

    heap_footer_t *block_footer = (heap_footer_t *)(orig_hole_pos + sizeof(heap_header_t) + size);
    block_footer->magic = KHEAP_MAGIC;
    block_footer->header = block_header;

    if (orig_hole_size - new_size > 0)
    {
        heap_header_t *hole_header = (heap_header_t *)(orig_hole_pos + sizeof(heap_header_t) + size + sizeof(heap_footer_t));
        hole_header->magic = KHEAP_MAGIC;
        hole_header->state = FREE;
        hole_header->size = orig_hole_size - new_size;
        heap_footer_t *hole_footer = (heap_footer_t *)((uint32_t)hole_header + orig_hole_size - new_size - sizeof(heap_footer_t));
        if ((uint32_t)hole_footer < heap->addr.end_address)
        {
            hole_footer->magic = KHEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        heap_array_insert_element((void *)hole_header, &heap->array);
    }
    return (void *)((uint32_t)block_header + sizeof(heap_header_t));
}

void kheap_free(void *ptr, heap_t *heap)
{
    if (ptr == 0)
        return;

    heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
    heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));

    assert(header->magic == KHEAP_MAGIC);
    assert(footer->magic == KHEAP_MAGIC);

    header->state = FREE;

    bool do_add = true;

    heap_footer_t *test_footer = (heap_footer_t *)((uint32_t)header - sizeof(heap_footer_t));
    if (test_footer->magic == KHEAP_MAGIC &&
        test_footer->header->state == FREE)
    {
        uint32_t cache_size = header->size;
        header = test_footer->header;
        footer->header = header;
        header->size += cache_size;
        do_add = false;
    }

    heap_header_t *test_header = (heap_header_t *)((uint32_t)footer + sizeof(heap_footer_t));
    if (test_header->magic == KHEAP_MAGIC &&
        test_header->state)
    {
        header->size += test_header->size;
        test_footer = (heap_footer_t *)((uint32_t)test_header +
                                        test_header->size - sizeof(heap_footer_t));
        footer = test_footer;

        uint32_t iterator = 0;
        while ((iterator < heap->array.size) &&
               (heap_array_get_element(iterator, &heap->array) != (data_t)test_header))
        {
            iterator++;
        }
        assert(iterator <= heap->array.size);
        heap_array_remove_element(iterator, &heap->array);
    }

    if ((uint32_t)footer + sizeof(heap_footer_t) == heap->addr.end_address)
    {
        uint32_t old_length = heap->addr.end_address - heap->addr.start_address;
        uint32_t new_length = __kheap_contract_heap((uint32_t)header - heap->addr.start_address, heap);

        if (header->size - (old_length - new_length) > 0)
        {
            header->size -= old_length - new_length;
            footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));
            footer->magic = KHEAP_MAGIC;
            footer->header = header;
        }
        else
        {
            uint32_t iterator = 0;
            while ((iterator < heap->array.size) &&
                   (heap_array_get_element(iterator, &heap->array) != (void *)test_header))
                iterator++;

            if (iterator < heap->array.size)
                heap_array_remove_element(iterator, &heap->array);
        }
    }

    if (do_add == true)
        heap_array_insert_element((void *)header, &heap->array);
}

uint32_t kheap_get_ptr_size(void *ptr)
{
    heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
    assert(header->magic == KHEAP_MAGIC);
    return (header->size - sizeof(heap_header_t) - sizeof(heap_footer_t));
}