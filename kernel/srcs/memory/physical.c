/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physical.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 16:23:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/03 21:33:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

uint32_t __page_placement_address = 0;

/*******************************************************************************
 *                                   KMALLOC                                   *
 ******************************************************************************/

void *kmalloc(size_t size)
{
    uint32_t tmp = __page_placement_address;

    __page_placement_address += size;
    __UNUSED__(tmp);
    __UNUSED__(size);
    return (NULL);
}

void *kmalloc_aligned(size_t size)
{
    __UNUSED__(size);
    return (NULL);
}


void kfree(void *ptr)
{
    (void)ptr;
}

size_t ksize(void *ptr)
{
    (void)ptr;
    return (0);
}

void *kbrk(void)
{
    return (NULL);

}