/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 00:33:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/27 12:23:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <kernel.h>
#include <memory/pmm.h>
#include <memory/memory_map.h>

#define SIZEOF_KBRK() (sizeof(void *))

#define PHYSICAL_START KMAP.available.start_addr
#define PHYSICAL_END KMAP.available.end_addr
#define PHYSICAL_LENGTH KMAP.available.length

#define PHYSICAL_EXPAND_HEAP_SIZE 0x1000

#define PHYSICAL_EXPAND_HEAP(current_end, size) (void *)(current_end + (pmm_get_next_available_block() * PMM_BLOCK_SIZE) + size)

typedef void data_t;

enum e_heap_block_state
{
    HEAP_BLOCK_FREE,
    HEAP_BLOCK_USED
};

typedef struct s_heap_block
{
    struct
    {
        uint32_t size;
        enum e_heap_block_state state;
    } metadata;
    struct s_heap_block *next;
    data_t *data;
} __attribute((packed)) t_heap_block;

typedef struct s_heap
{
    data_t *start_addr;
    data_t *end_addr;
    uint32_t max_size;
    uint32_t used_size;
    t_heap_block *root;
} __attribute((packed)) t_heap;

#define HeapBlock t_heap_block
#define Heap t_heap

extern Heap kheap;

extern int kheap_init(void *start_addr, void *end_addr);
extern data_t *kbrk(uint32_t size);
extern data_t *kmalloc(uint32_t size);
extern data_t *krealloc(void *ptr, uint32_t size);
extern uint32_t ksize(data_t *ptr);
extern void kfree(void *ptr);
extern void *kcalloc(uint32_t count, uint32_t size);
extern data_t *kmmap(uint32_t size);

#endif /* !KHEAP_H */