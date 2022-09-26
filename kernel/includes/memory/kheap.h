/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 00:33:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/14 00:45:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <kernel.h>

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
    struct s_heap *next;
    void *data;
} __attribute((packed)) t_heap_block;

typedef struct s_heap
{
    void *start_addr;
    void *end_addr;
    uint32_t max_size;
    uint32_t used_size;
    t_heap_block *root;
} __attribute((packed)) t_heap;

#define HeapBlock t_heap_block
#define Heap t_heap

extern Heap kheap;

extern int kheap_init(void *start_addr, void *end_addr);

#endif /* !KHEAP_H */