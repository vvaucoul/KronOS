/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 00:17:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/02 10:46:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h> // Early Alloc (ealloc, ealloc_aligned)
#include <mm/kheap.h>  // Kernel Heap
#include <mm/mm.h>	   // Memory Management
#include <mm/mmu.h>	   // Memory Management Unit (MMU / Paging)
#include <mm/mmuf.h>   // Memory Management Unit Frames (MMU Frames)

#include <assert.h> // assert
#include <stddef.h> // NULL

static heap_t *kheap = NULL;

static int64_t __kheap_find_hole(uint32_t size, bool align, heap_t *heap) {
	int64_t index = 0;

	while ((uint32_t)index < heap->array.size) {
		heap_header_t *header = (heap_header_t *)heap_array_get_element(index, &heap->array);

		if (align == true) {
			uint32_t location = (uint32_t)header;
			int32_t offset = 0;

			if (((location + sizeof(heap_header_t)) & 0xFFFFF000) != 0)
				offset = PAGE_SIZE - (location + sizeof(heap_header_t)) % PAGE_SIZE;

			uint32_t hole_size = (header->size - offset);

			if (hole_size >= size)
				break;
		} else if (header->size >= size)
			break;
		index++;
	}

	if ((uint32_t)index == heap->array.size)
		return (-1);
	else
		return (index);
}

static void __kheap_expand_heap(uint32_t new_size, heap_t *heap) {
	assert(new_size > heap->addr.end_address - heap->addr.start_address);

	if ((new_size & 0xFFFFF000) != 0) {
		new_size &= 0xFFFFF000;
		new_size += PAGE_SIZE;
	}

	assert(heap->addr.start_address + new_size <= heap->addr.max_address);

	uint32_t old_size = heap->addr.end_address - heap->addr.start_address;
	uint32_t i = old_size;

	while (i < new_size) {
		const page_t *page = mmu_get_page(heap->addr.start_address + i, mmu_get_kernel_directory());
		if (!page)
			page = mmu_create_page(heap->addr.start_address + i, mmu_get_kernel_directory());
		allocate_frame((page_t *)page, (heap->flags.supervisor) ? 1 : 0, (heap->flags.readonly) ? 0 : 1);
		i += PAGE_SIZE;
	}
	heap->addr.end_address = heap->addr.start_address + new_size;
}

static uint32_t __kheap_contract_heap(uint32_t new_size, heap_t *heap) {
	assert(new_size < heap->addr.end_address - heap->addr.start_address);

	if ((new_size & PAGE_SIZE) != 0) {
		new_size &= PAGE_SIZE;
		new_size += PAGE_SIZE;
	}

	if (new_size < HEAP_MIN_SIZE)
		new_size = HEAP_MIN_SIZE;

	uint32_t old_size = heap->addr.end_address - heap->addr.start_address;
	uint32_t i = old_size - PAGE_SIZE;

	while (new_size < i) {
		const page_t *page = mmu_get_page(heap->addr.start_address + i, mmu_get_kernel_directory());
		assert(page != NULL);

		free_frame((page_t *)page);
		i -= PAGE_SIZE;
	}
	heap->addr.end_address = heap->addr.start_address + new_size;
	return (new_size);
}

data_t kheap_alloc(uint32_t size, bool align, heap_t *heap) {
	uint32_t new_size = size + sizeof(heap_header_t) + sizeof(heap_footer_t);
	int32_t iterator = __kheap_find_hole(new_size, align, heap);

	if (iterator == -1) {
		uint32_t old_length = heap->addr.end_address - heap->addr.start_address;
		uint32_t old_end_address = heap->addr.end_address;

		__kheap_expand_heap(old_length + new_size, heap);
		uint32_t new_length = heap->addr.end_address - heap->addr.start_address;

		iterator = 0;

		uint32_t idx = -1;
		uint32_t value = 0x0;
		while ((uint32_t)iterator < heap->array.size) {
			uint32_t tmp = (uint32_t)heap_array_get_element(iterator, &heap->array);
			if (tmp > value) {
				value = tmp;
				idx = iterator;
			}
			iterator++;
		}

		if (idx == 0xFFFFFFFF) {
			heap_header_t *header = (heap_header_t *)old_end_address;
			header->magic = HEAP_MAGIC;
			header->size = new_length - old_length;
			header->state = FREE;
			heap_footer_t *footer = (heap_footer_t *)(old_end_address + header->size - sizeof(heap_footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			heap_array_insert_element((void *)header, &heap->array);
		} else {
			heap_header_t *header = heap_array_get_element(idx, &heap->array);
			header->size += new_length - old_length;

			heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}

		return kheap_alloc(size, align, heap);
	}

	heap_header_t *orig_hole_header = (heap_header_t *)heap_array_get_element(iterator, &heap->array);
	uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
	uint32_t orig_hole_size = orig_hole_header->size;

	if (orig_hole_size - new_size < sizeof(heap_header_t) + sizeof(heap_footer_t)) {
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}

	if (align && orig_hole_pos & 0xFFFFF000) {
		uint32_t new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(heap_header_t);
		heap_header_t *hole_header = (heap_header_t *)orig_hole_pos;
		hole_header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(heap_header_t);
		hole_header->magic = HEAP_MAGIC;
		hole_header->state = FREE;
		heap_footer_t *hole_footer = (heap_footer_t *)((uint32_t)new_location - sizeof(heap_footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size - hole_header->size;
	} else {
		heap_array_remove_element(iterator, &heap->array);
	}

	heap_header_t *block_header = (heap_header_t *)orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->state = USED;
	block_header->size = new_size;

	heap_footer_t *block_footer = (heap_footer_t *)(orig_hole_pos + sizeof(heap_header_t) + size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;

	if (orig_hole_size - new_size > 0) {
		heap_header_t *hole_header = (heap_header_t *)(orig_hole_pos + sizeof(heap_header_t) + size + sizeof(heap_footer_t));
		hole_header->magic = HEAP_MAGIC;
		hole_header->state = FREE;
		hole_header->size = orig_hole_size - new_size;
		heap_footer_t *hole_footer = (heap_footer_t *)((uint32_t)hole_header + orig_hole_size - new_size - sizeof(heap_footer_t));
		if ((uint32_t)hole_footer < heap->addr.end_address) {
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;
		}
		heap_array_insert_element((void *)hole_header, &heap->array);
	}
	return (void *)((uint32_t)block_header + sizeof(heap_header_t));
}

void kheap_free(void *ptr, heap_t *heap) {
	if (ptr == 0)
		return;

	heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
	heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));

	assert(header->magic == HEAP_MAGIC);
	assert(footer->magic == HEAP_MAGIC);

	header->state = FREE;

	bool do_add = true;

	heap_footer_t *test_footer = (heap_footer_t *)((uint32_t)header - sizeof(heap_footer_t));
	if (test_footer->magic == HEAP_MAGIC &&
		test_footer->header->state == FREE) {
		uint32_t cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = false;
	}

	heap_header_t *test_header = (heap_header_t *)((uint32_t)footer + sizeof(heap_footer_t));
	if (test_header->magic == HEAP_MAGIC &&
		test_header->state) {
		header->size += test_header->size;
		test_footer = (heap_footer_t *)((uint32_t)test_header +
										test_header->size - sizeof(heap_footer_t));
		footer = test_footer;

		uint32_t iterator = 0;
		while ((iterator < heap->array.size) &&
			   (heap_array_get_element(iterator, &heap->array) != (data_t)test_header)) {
			iterator++;
		}
		assert(iterator <= heap->array.size);
		heap_array_remove_element(iterator, &heap->array);
	}

	if ((uint32_t)footer + sizeof(heap_footer_t) == heap->addr.end_address) {
		uint32_t old_length = heap->addr.end_address - heap->addr.start_address;
		uint32_t new_length = __kheap_contract_heap((uint32_t)header - heap->addr.start_address, heap);

		if (header->size - (old_length - new_length) > 0) {
			header->size -= old_length - new_length;
			footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
		} else {
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

uint32_t kheap_get_ptr_size(void *ptr) {
	heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
	assert(header->magic == HEAP_MAGIC);
	return (header->size - sizeof(heap_header_t) - sizeof(heap_footer_t));
}

static uint8_t header_t_less_than(void *a, void *b) {
	return (((heap_header_t *)a)->size < ((heap_header_t *)b)->size) ? 1 : 0;
}

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max) {
	heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

	heap->array = heap_array_create((void *)start, HEAP_INDEX_SIZE, &header_t_less_than);

	start += sizeof(data_t) * HEAP_INDEX_SIZE;

	if (start & 0xFFFFF000) {
		start &= 0xFFFFF000;
		start += 0x1000;
	}

	heap->addr.start_address = start;
	heap->addr.end_address = end;
	heap->addr.max_address = max;
	heap->flags.supervisor = 0; // User mode
	heap->flags.readonly = 0;	// Read/Write

	heap_header_t *hole = (heap_header_t *)start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->state = FREE;
	heap_array_insert_element((void *)hole, &heap->array);

	kheap = heap;
	return (heap);
}

extern uint32_t placement_addr;

static void *intermediate_alloc(size_t size, uint8_t align, uint32_t *phys) {
	/* If the kernel heap is initialized, we use it */
	if (kheap) {
		void *addr = kheap_alloc(size, align, kheap);
		if (phys) {
			page_t *page = mmu_get_page((uint32_t)addr, mmu_get_kernel_directory());
			if (page)
				*phys = page->frame * PAGE_SIZE + ((uint32_t)addr & 0xFFF);
			else
				*phys = 0;
		}
		return addr;
	}
	/* If the kernel heap is not initialized, we use the early alloc */
	else {
		if (align && (placement_addr & 0xFFFFF000)) {
			placement_addr &= 0xFFFFF000;
			placement_addr += PAGE_SIZE;
		}
		if (phys) {
			*phys = placement_addr;
		}
		/* Don't need to store data in block -> reverved for system */
		return (ealloc(size));
	}
}

void *kmalloc(size_t size) {
	return intermediate_alloc(size, 0, NULL);
}

void *kmalloc_a(size_t size) {
	return intermediate_alloc(size, 1, NULL);
}

void *kmalloc_p(size_t size, uint32_t *phys) {
	return intermediate_alloc(size, 0, phys);
}

void *kmalloc_ap(size_t size, uint32_t *phys) {
	return intermediate_alloc(size, 1, phys);
}

void kfree(void *p) {
	if (kheap) {
		kheap_free(p, kheap);
	}
}

void *kcalloc(size_t num, size_t size) {
	void *p = kmalloc(num * size);
	memset(p, 0, num * size);
	return p;
}

void *krealloc(void *p, size_t size) {
	void *new = kmalloc(size);
	size_t old_size = ksize(p);
	if (old_size < size) {
		memcpy(new, p, old_size);
	} else {
		memcpy(new, p, size);
	}
	kfree(p);
	return new;
}

size_t ksize(void *p) {
	if (kheap) {
		return kheap_get_ptr_size(p);
	}
	return 0;
}