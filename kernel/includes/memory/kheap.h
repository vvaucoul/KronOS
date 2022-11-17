/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/17 17:36:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <kernel.h>

#include <memory/memory.h>
#include <memory/memory_map.h>

#define KHEAP_START 0xBFFE0000
#define KHEAP_INITIAL_SIZE 0x100000

typedef struct s_heap
{
    int test;
} t_heap;

typedef t_heap heap_t;

extern heap_t *kheap;
extern uint32_t placement_addr;

/*******************************************************************************
 *                             INTERFACE FUNCTIONS                             *
 ******************************************************************************/

extern uint32_t kmalloc_int(uint32_t size, int align, uint32_t *phys);
extern uint32_t kmalloc_a(uint32_t size);
extern uint32_t kmalloc_p(uint32_t size, uint32_t *phys);
extern uint32_t kmalloc_ap(uint32_t size, uint32_t *phys);

extern uint32_t kmalloc(uint32_t size);
extern uint32_t krealloc(void *ptr, uint32_t size);
extern uint32_t kcalloc(void *ptr, uint32_t size);

extern void kfree(void *ptr);

extern void *kbrk(uint32_t size);

extern void init_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr, uint32_t supervisor, uint32_t readonly);

#endif /* !KHEAP_H */