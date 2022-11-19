/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/19 11:56:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <kernel.h>

#include <memory/memory.h>
#include <memory/memory_map.h>

#include <memory/ordered_array.h>

#define KHEAP_START 0xC0000000
#define KHEAP_MAX_SIZE 0xCFFFF000
#define KHEAP_INITIAL_SIZE 0x100000
#define KHEAP_MAGIC 0x123890AB
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MIN_SIZE 0x70000
#define PHYSICAL_MEMORY_SIZE 0x1000000

/* Split blocks into 3 heaps */
#define KHEAP_SIZE_LOW 0x1000
#define KHEAP_SIZE_MEDIUM 0x10000
#define KHEAP_SIZE_HIGH 0x100000

#define KHEAP_BLOCK_SIZE_LOW (KHEAP_SIZE_LOW / 32)
#define KHEAP_BLOCK_SIZE_MEDIUM (KHEAP_SIZE_MEDIUM / 32)
#define KHEAP_BLOCK_SIZE_HIGH (KHEAP_SIZE_HIGH / 32)

#define KHEAP_INDEX_LOW 0x0
#define KHEAP_INDEX_MEDIUM 0x1
#define KHEAP_INDEX_HIGH 0x2

#define KHEAP_SIZE 0x3

enum kheap_block_status
{
    USED,
    FREE
};

enum kheap_block_size
{
    LOW,
    MEDIUM,
    HIGH
};

/* Magic Number: Sentinel Number 
** --> 0x123890AB
*/

typedef struct s_heap_header
{
    uint32_t magic;
    enum kheap_block_status status;
    uint32_t size;
} heap_header_t;

typedef struct s_heap_footer
{
    uint32_t magic;
    heap_header_t *header;
} heap_footer_t;

// typedef struct s_heap_node
// {
//     struct
//     {
//         struct s_heap_node *parent;
//         struct s_heap_node *left;
//         struct s_heap_node *right;
//     } tree;

//     struct
//     {
//         uint32_t addr;
//         enum kheap_block_status status;
//         enum kheap_block_size block_size;
//         uint32_t size;
//     } infos;

//     void *data;
// } heap_node_t;

// typedef struct s_heap
// {
//     heap_node_t *root;
//     struct
//     {
//         uint32_t start_address;
//         uint32_t end_address;
//         uint32_t max_address;
//     } addr;

//     struct
//     {
//         uint32_t max_blocks;
//         uint32_t used_blocks;
//     } blocks;
// } heap_t;

typedef struct s_heap
{
    ordered_array_t index;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_address;
    uint8_t supervisor;
    uint8_t readonly;
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

extern void *kmalloc(uint32_t size);
extern void *krealloc(void *ptr, uint32_t size);
extern void *kcalloc(void *ptr, uint32_t size);

extern void kfree(void *ptr);

extern void *kbrk(uint32_t size);

extern uint32_t ksize(void *ptr);

extern void init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly);

extern void *alloc(uint32_t size, uint8_t align, heap_t *heap);
extern void free(void *p, heap_t *heap);

// extern void *kheap_tree_alloc_memory(uint32_t size);
// extern void kheap_tree_delete_node(void *ptr);
// extern uint32_t kheap_tree_get_node_size(void *ptr);

#endif /* !KHEAP_H */