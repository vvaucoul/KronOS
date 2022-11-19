/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 15:27:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 16:54:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

uint32_t placement_address = (uint32_t)&_end;

static uint32_t kmalloc_internal(uint32_t size, int align, void *physical_addr)
{
    if (align == 1 && (placement_address & 0xFFFFF000))
    {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (physical_addr)
        *(uint32_t *)physical_addr = placement_address;
    uint32_t tmp = placement_address;
    placement_address += size;
    return (tmp);
}

uint32_t kmalloc_a(uint32_t size)
{
    return (kmalloc_internal(size, 1, 0));
}

uint32_t kmalloc_p(uint32_t size, void *physical_addr)
{
    return (kmalloc_internal(size, 0, physical_addr));
}

uint32_t kmalloc_ap(uint32_t size, void *physical_addr)
{
    return (kmalloc_internal(size, 1, physical_addr));
}

uint32_t kmalloc(uint32_t size)
{
    return (kmalloc_internal(size, 0, 0));
}