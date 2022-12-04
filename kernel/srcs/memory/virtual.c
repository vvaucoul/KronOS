/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:49:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/04 15:32:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

void *vmalloc(uint32_t size)
{
    uint32_t phys = 0;
    void *addr = kmalloc_ap(size, &phys);

    if (!addr)
        return (NULL);
    return (addr);
    // else if (!phys)
    //     return (NULL);
    // return ((void *)phys);
}

void vfree(void *addr)
{
    kfree(addr);
}

void *vrealloc(void *addr, uint32_t size)
{
    void *new_addr = vmalloc(size);
    void *old_addr = get_physical_address(addr);

    if (!new_addr)
        return (NULL);
    memcpy(new_addr, old_addr, size);
    kfree(old_addr);
    return (new_addr);
}

void *vcalloc(uint32_t size)
{
    void *addr = vmalloc(size);

    if (!addr)
        return (NULL);
    memset(addr, 0, size);
    return (addr);
}

uint32_t vsize(void *addr)
{
    return (ksize(addr));
}

void *vbrk(uint32_t size)
{
    return (kbrk(size));
}
