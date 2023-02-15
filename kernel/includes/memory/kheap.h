/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 11:30:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <kernel.h>

#include <memory/memory.h>
#include <memory/memory_map.h>

#define KHEAP_START 0xC0000000
#define KHEAP_MAX_SIZE 0xCFFFF000
#define KHEAP_INITIAL_SIZE 0x100000
#define KHEAP_MAGIC 0x123890AB
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MIN_SIZE 0x70000
#define PHYSICAL_MEMORY_SIZE 0x1000000

enum kheap_block_status
{
    USED,
    FREE
};

/* Magic Number: Sentinel Number
** --> 0x123890AB
*/

typedef void *data_t;

typedef struct s_heap_header
{
    uint32_t magic;
    enum kheap_block_status state;
    uint32_t size;
} heap_header_t;

typedef struct s_heap_footer
{
    uint32_t magic;
    heap_header_t *header;
} heap_footer_t;

typedef bool (*heap_node_predicate_t)(data_t, data_t);

typedef struct s_heap_array
{
    data_t *array;
    uint32_t size;
    uint32_t max_size;
    heap_node_predicate_t predicate;
} heap_array_t;

typedef struct s_heap
{
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

extern heap_t *kheap;
extern uint32_t placement_addr;

/*******************************************************************************
 *                             INTERFACE FUNCTIONS                             *
 ******************************************************************************/

extern void *kmalloc_int(uint32_t size, bool align, uint32_t *phys);
extern void *kmalloc_a(uint32_t size);
extern void *kmalloc_p(uint32_t size, uint32_t *phys);
extern void *kmalloc_ap(uint32_t size, uint32_t *phys);
extern void *kmalloc_v(uint32_t size);

extern void *kmalloc(uint32_t size);
extern void *krealloc(void *ptr, uint32_t size);
extern void *kcalloc(uint32_t count, uint32_t size);

extern void kfree(void *ptr);
extern void kfree_v(void *ptr);
extern void kfree_p(void *ptr);

extern void *kbrk(uint32_t size);

extern uint32_t ksize(void *ptr);

extern void init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly);

extern data_t kheap_alloc(uint32_t size, bool align, heap_t *heap);
extern void kheap_free(void *ptr, heap_t *heap);
extern uint32_t kheap_get_ptr_size(void *ptr);

extern void *vmalloc(uint32_t size);
extern void *vbrk(uint32_t size);
extern void vfree(void *addr);
extern void *vrealloc(void *addr, uint32_t size);
extern void *vcalloc(uint32_t count, uint32_t size);
extern uint32_t vsize(void *addr);

/*******************************************************************************
 *                                 HEAP ARRAY                                  *
 ******************************************************************************/

extern heap_array_t heap_array_create(void *addr, uint32_t max_size, heap_node_predicate_t predicate);
extern void heap_array_insert_element(data_t data, heap_array_t *array);
extern data_t heap_array_get_element(uint32_t index, heap_array_t *array);
extern void heap_array_remove_element(uint32_t index, heap_array_t *array);
extern void heap_destroy(heap_array_t *array);
extern bool heap_predicate(data_t a, data_t b);

#endif /* !KHEAP_H */