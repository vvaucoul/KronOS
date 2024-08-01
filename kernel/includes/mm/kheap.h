/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:53:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <stdbool.h> // bool
#include <stddef.h>	 // size_t
#include <stdint.h>	 // uint32_t

#define HEAP_MAGIC 0x12345678			   // Magic number for heap header
#define HEAP_MIN_SIZE 0x70000			   // 448KB
#define KERNEL_HEAP_EXPAND_OFFSET 0x100000 // 1MB

#define HEAP_START 0xC0000000
#define HEAP_INITIAL_SIZE 0x100000 // 1MB
#define HEAP_MAX_SIZE 0xCFFFF000	// ~3GB
#define HEAP_INDEX_SIZE 0x20000	// 128KB

enum kheap_block_status {
	USED,
	FREE
};

typedef void *data_t;
typedef struct s_heap_header {
	uint32_t magic;
	enum kheap_block_status state;
	uint32_t size;
} heap_header_t;

typedef struct s_heap_footer {
	uint32_t magic;
	heap_header_t *header;
} heap_footer_t;

typedef bool (*heap_node_predicate_t)(data_t, data_t);

typedef struct s_heap_array {
	data_t *array;
	uint32_t size;
	uint32_t max_size;
	heap_node_predicate_t predicate;
} heap_array_t;

typedef struct s_heap {
	heap_array_t array;
	struct
	{
		uint32_t start_address;
		uint32_t end_address;
		uint32_t max_address;
	} addr;

	struct
	{
		uint8_t supervisor;
		uint8_t readonly;
	} flags;
} heap_t;

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max);

void *kmalloc(size_t size);
void *kmalloc_a(size_t size);
void *kmalloc_p(size_t size, uint32_t *phys);
void *kmalloc_ap(size_t size, uint32_t *phys);

void kfree(void *p);
void *kcalloc(size_t num, size_t size);
void *krealloc(void *p, size_t size);

size_t ksize(void *p);

void *vmalloc(size_t size);
void vfree(void *p);
void *vcalloc(size_t num, size_t size);
void *vrealloc(void *p, size_t size);

size_t vsize(void *p);

bool heap_predicate(data_t a, data_t b);
heap_array_t heap_array_create(void *addr, uint32_t max_size, heap_node_predicate_t predicate);
void heap_array_insert_element(data_t data, heap_array_t *array);
data_t heap_array_get_element(uint32_t index, heap_array_t *array);
void heap_array_remove_element(uint32_t index, heap_array_t *array);
void heap_destroy(heap_array_t *array);

#endif /* !KHEAP_H */