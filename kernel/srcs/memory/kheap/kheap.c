/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:09:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/04 14:23:07 by vvaucoul         ###   ########.fr       */
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

    /* Contract Limit */
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
    // Make sure we take the size of header/footer into account.
    uint32_t new_size = size + sizeof(heap_header_t) + sizeof(heap_footer_t);
    // Find the smallest hole that will fit.
    int32_t iterator = __kheap_find_hole(new_size, align, heap);

    if (iterator == -1) // If we didn't find a suitable hole
    {
        // Save some previous data.
        uint32_t old_length = heap->addr.end_address - heap->addr.start_address;
        uint32_t old_end_address = heap->addr.end_address;

        // We need to allocate some more space.
        __kheap_expand_heap(old_length + new_size, heap);
        uint32_t new_length = heap->addr.end_address - heap->addr.start_address;

        // Find the endmost header. (Not endmost in size, but in location).
        iterator = 0;
        // Vars to hold the index of, and value of, the endmost header found so far.
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

        // If we didn't find ANY headers, we need to add one.
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
            // The last header needs adjusting.
            heap_header_t *header = heap_array_get_element(idx, &heap->array);
            header->size += new_length - old_length;
            // Rewrite the footer.
            heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));
            footer->header = header;
            footer->magic = KHEAP_MAGIC;
        }
        // We now have enough space. Recurse, and call the function again.
        return kheap_alloc(size, align, heap);
    }

    heap_header_t *orig_hole_header = (heap_header_t *)heap_array_get_element(iterator, &heap->array);
    uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
    uint32_t orig_hole_size = orig_hole_header->size;
    // Here we work out if we should split the hole we found into two parts.
    // Is the original hole size - requested hole size less than the overhead for adding a new hole?
    if (orig_hole_size - new_size < sizeof(heap_header_t) + sizeof(heap_footer_t))
    {
        // Then just increase the requested size to the size of the hole we found.
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }

    // If we need to page-align the data, do it now and make a new hole in front of our block.
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
        // Else we don't need this hole any more, delete it from the index.
        heap_array_remove_element(iterator, &heap->array);
    }

    // Overwrite the original header...
    heap_header_t *block_header = (heap_header_t *)orig_hole_pos;
    block_header->magic = KHEAP_MAGIC;
    block_header->state = USED;
    block_header->size = new_size;
    // ...And the footer
    heap_footer_t *block_footer = (heap_footer_t *)(orig_hole_pos + sizeof(heap_header_t) + size);
    block_footer->magic = KHEAP_MAGIC;
    block_footer->header = block_header;

    // We may need to write a new hole after the allocated block.
    // We do this only if the new hole would have positive size...
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
        // Put the new hole in the index;
        heap_array_insert_element((void *)hole_header, &heap->array);
    }

    // ...And we're done!
    return (void *)((uint32_t)block_header + sizeof(heap_header_t));
}

void kheap_free(void *ptr, heap_t *heap)
{
    // Exit gracefully for null pointers.
    if (ptr == 0)
        return;

    // Get the header and footer associated with this pointer.
    heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
    heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));

    // Sanity checks.
    assert(header->magic == KHEAP_MAGIC);
    assert(footer->magic == KHEAP_MAGIC);

    // Make us a hole.
    header->state = FREE;

    // Do we want to add this header into the 'free holes' index?
    bool do_add = 1;
    
    // Unify left
    // If the thing immediately to the left of us is a footer...
    heap_footer_t *test_footer = (heap_footer_t *)((uint32_t)header - sizeof(heap_footer_t));
    if (test_footer->magic == KHEAP_MAGIC &&
        test_footer->header->state == FREE)
    {
        uint32_t cache_size = header->size; // Cache our current size.
        header = test_footer->header;       // Rewrite our header with the new one.
        footer->header = header;            // Rewrite our footer to point to the new header.
        header->size += cache_size;         // Change the size.
        do_add = 0;                         // Since this header is already in the index, we don't want to add it again.        
    }

    // Unify right
    // If the thing immediately to the right of us is a header...
    heap_header_t *test_header = (heap_header_t *)((uint32_t)footer + sizeof(heap_footer_t));
    if (test_header->magic == KHEAP_MAGIC &&
        test_header->state)
    {
        header->size += test_header->size;                      // Increase our size.
        test_footer = (heap_footer_t *)((uint32_t)test_header + // Rewrite it's footer to point to our header.
                                        test_header->size - sizeof(heap_footer_t));
        footer = test_footer;
        // Find and remove this header from the index.
        uint32_t iterator = 0;        
        while ((iterator < heap->array.size) &&
               (heap_array_get_element(iterator, &heap->array) != (data_t)test_header))
        {
            iterator++;
        }
        
        // Make sure we actually found the item.
        assert(iterator <= heap->array.size);        
        // Remove it.        
        heap_array_remove_element(iterator, &heap->array);                
    }

    // If the footer location is the end address, we can contract.
    if ((uint32_t)footer + sizeof(heap_footer_t) == heap->addr.end_address)
    {
        uint32_t old_length = heap->addr.end_address - heap->addr.start_address;
        uint32_t new_length = __kheap_contract_heap((uint32_t)header - heap->addr.start_address, heap);
        // Check how big we will be after resizing.
        if (header->size - (old_length - new_length) > 0)
        {
            // We will still exist, so resize us.
            header->size -= old_length - new_length;
            footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));
            footer->magic = KHEAP_MAGIC;
            footer->header = header;
        }
        else
        {
            // We will no longer exist :(. Remove us from the index.
            uint32_t iterator = 0;
            while ((iterator < heap->array.size) &&
                   (heap_array_get_element(iterator, &heap->array) != (void *)test_header))
                iterator++;
            // If we didn't find ourselves, we have nothing to remove.
            if (iterator < heap->array.size)
                heap_array_remove_element(iterator, &heap->array);
        }        
    }

    // If required, add us to the index.    
    if (do_add == 1)
        heap_array_insert_element((void *)header, &heap->array);
}

uint32_t kheap_get_ptr_size(void *ptr)
{
    heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
    assert(header->magic == KHEAP_MAGIC);
    return (header->size - sizeof(heap_header_t) - sizeof(heap_footer_t));
}