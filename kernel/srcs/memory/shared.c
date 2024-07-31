/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 12:28:12 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 11:27:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>
#include <memory/shared.h>

static void *__kmalloc_shared(uint32_t size, bool align, heap_t *heap) {
	uint32_t new_size = size + sizeof(shared_heap_header_t) + sizeof(heap_footer_t);
	shared_heap_header_t *shared_header = (shared_heap_header_t *)kheap_alloc(new_size, align, heap);
	shared_header->ref_count = 1; // Initialize reference count
	return (void *)((uint32_t)shared_header + sizeof(shared_heap_header_t));
}

static void __kheap_free_shared(void *ptr, heap_t *heap) {
	if (ptr == 0)
		return;
	shared_heap_header_t *shared_header = (shared_heap_header_t *)((uint32_t)ptr - sizeof(shared_heap_header_t));
	shared_header->ref_count--; // Decrease reference count
	if (shared_header->ref_count == 0) {
		kheap_free((void *)shared_header, heap);
	}
}

static void *__kdup_shared(void *ptr) {
	shared_heap_header_t *shared_header = (shared_heap_header_t *)((uint32_t)ptr - sizeof(shared_heap_header_t));
	shared_header->ref_count++;
	return ptr;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                               INTERFACE FUNCTIONS                              ||
// ! ||--------------------------------------------------------------------------------||

void *kdup_shared(void *ptr) {
	return (__kdup_shared(ptr));
}

void *kmalloc_shared(uint32_t size) {
	return (__kmalloc_shared(size, 0, get_kheap()));
}

void kfree_shared(void *ptr) {
	__kheap_free_shared(ptr, get_kheap());
}