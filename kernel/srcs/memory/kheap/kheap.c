/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/24 14:30:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>
#include <memory/paging.h>
#include <memory/shared.h>

#include <system/panic.h>

uint32_t placement_addr = (uint32_t)(uint32_t *)(&__kernel_section_end);
heap_t *kheap = NULL;

static void *__kbrk(uint32_t size) {
    void *addr = NULL;

    if (size == 0)
        return (NULL);
    else {
        addr = (void *)placement_addr;
        placement_addr += size;
    }
    return (addr);
}

static void *__kbrk_int(uint32_t size, bool align, uint32_t *phys) {
    void *addr = NULL;

    if (align && (placement_addr & 0xFFFFF000)) {
        placement_addr &= 0xFFFFF000;
        placement_addr += 0x1000;
    }
    if (phys) {
        *phys = placement_addr;
    }
    addr = (void *)placement_addr;
    placement_addr += size;
    return (addr);
}

static void *__kmalloc_int(uint32_t size, bool align, uint32_t *phys) {
    /* If Heap exists -> Heap Algorithm with Virtual Memory */
    if (kheap) {
        void *addr = kheap_alloc(size, align, kheap);
        if (phys) {
            page_t *page = get_page((uint32_t)addr, kernel_directory);
            if (page)
                *phys = page->frame * PAGE_SIZE + ((uint32_t)addr & 0xFFF);
            else
                *phys = 0;
        }
        if (align) {
            if ((placement_addr & 0xFFFFF000)) {
                placement_addr &= 0xFFFFF000;
                placement_addr += PAGE_SIZE;
            }
        }
        return (addr);
    }
    /* Either, use physical Memory */
    else {
        if (align && (placement_addr & 0xFFFFF000)) {
            placement_addr &= 0xFFFFF000;
            placement_addr += PAGE_SIZE;
        }
        if (phys) {
            *phys = placement_addr;
        }
        /* Don't need to store data in block -> reverved for system */
        return (__kbrk(size));
    }
}

static uint32_t __ksize(void *ptr) {
    return (kheap_get_ptr_size(ptr));
}

static void *__krealloc(void *ptr, uint32_t size) {
    void *new_ptr = NULL;

    if (ptr == NULL)
        return (NULL);
    else {
        new_ptr = kmalloc(size);
        if (new_ptr == NULL)
            return (NULL);
        memcpy(new_ptr, ptr, size);
        kfree(ptr);
    }
    return (new_ptr);
}

static void *__kcalloc(uint32_t count, uint32_t size) {
    void *ptr = kmalloc(count * size);

    if (ptr == NULL)
        return (NULL);
    memset(ptr, 0, count * size);
    return (ptr);
}

static void __kfree(void *ptr) {
    if (kheap)
        kheap_free(ptr, kheap);
}

static uint8_t header_t_less_than(void *a, void *b) {
    return (((heap_header_t *)a)->size < ((heap_header_t *)b)->size) ? 1 : 0;
}

static heap_t *__init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly) {
    if (!IS_ALIGNED(start_addr) || !IS_ALIGNED(end_addr))
        __PANIC("KHEAP : Start and End address must be aligned to 0x1000");

    assert(start_addr % 0x1000 == 0);
    assert(end_addr % 0x1000 == 0);

    heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

    heap->array = heap_array_create((void *)start_addr, HEAP_INDEX_SIZE, &header_t_less_than);

    start_addr += sizeof(data_t) * HEAP_INDEX_SIZE;

    if (start_addr & 0xFFFFF000) {
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

void init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly) {
    kheap = __init_heap(start_addr, end_addr, max_addr, supervisor, readonly);
    assert(kheap != NULL);
}

void *kmalloc_int(uint32_t size, bool align, uint32_t *phys) {
    return (__kmalloc_int(size, align, phys));
}

void *kmalloc_a(uint32_t size) {
    return (__kmalloc_int(size, 1, 0));
}

void *kmalloc_p(uint32_t size, uint32_t *phys) {
    return (__kmalloc_int(size, 0, phys));
}

void *kmalloc_ap(uint32_t size, uint32_t *phys) {
    return (__kmalloc_int(size, 1, phys));
}

void *kmalloc(uint32_t size) {
    return (__kmalloc_int(size, 0, 0));
}

void *kmalloc_v(uint32_t size) {
    return (__kmalloc_int(size, 0, 0));
}

void kfree_v(void *ptr) {
    __kfree(ptr);
}

void kfree_p(void *ptr) {
    __kfree(get_virtual_address(kernel_directory, ptr));
}

void kfree(void *ptr) {
    __kfree(ptr);
}

void *krealloc(void *ptr, uint32_t size) {
    return (__krealloc(ptr, size));
}

void *kcalloc(uint32_t count, uint32_t size) {
    return (__kcalloc(count, size));
}

void *kbrk(uint32_t size) {
    return (__kbrk(size));
}

void *kbrk_int(uint32_t size, bool align, uint32_t *phys) {
    return (__kbrk_int(size, align, phys));
}

void *kbrk_a(uint32_t size) {
    return (__kbrk_int(size, 1, 0));
}

void *kbrk_p(uint32_t size, uint32_t *phys) {
    return (__kbrk_int(size, 0, phys));
}

void *kbrk_ap(uint32_t size, uint32_t *phys) {
    return (__kbrk_int(size, 1, phys));
}

void *kbrk_v(uint32_t size) {
    return (__kbrk_int(size, 0, 0));
}

uint32_t ksize(void *ptr) {
    return (__ksize(ptr));
}