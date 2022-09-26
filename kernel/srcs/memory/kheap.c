/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 00:33:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/14 01:14:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

Heap kheap;

/*******************************************************************************
 *                           PRIVATE HEAP FUNCTIONS                            *
 ******************************************************************************/

static void __expand_heap(Heap *heap, uint32_t size)
{
}

static int __init(void *start_addr, void *end_addr)
{
    if (!start_addr || !end_addr)
        return (1);
    else if (start_addr >= end_addr)
        return (1);
    else
    {
        kheap.start_addr = start_addr;
        kheap.end_addr = end_addr;
        kheap.max_size = end_addr - start_addr;
        kheap.used_size = 0;
        kheap.root = NULL;
    }
    return (0);
}

/*******************************************************************************
 *                            GLOBAL HEAP FUNCTIONS                            *
 ******************************************************************************/

int kheap_init(void *start_addr, void *end_addr)
{
    return (__init(start_addr, end_addr));
}

void *kmalloc(uint32_t size)
{
    return (NULL);
}

void *krealloc(void *ptr, uint32_t size)
{
    return (NULL);
}

void kfree(void *ptr)
{
    return;
}

void *kcalloc(uint32_t size)
{
    return (NULL);
}

void *kbrk(uint32_t size)
{
    return (NULL);
}

void *kmmap(uint32_t size)
{
    return (NULL);
}