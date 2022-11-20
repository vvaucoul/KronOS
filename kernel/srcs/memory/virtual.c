/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:49:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:28:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

void *vmalloc(uint32_t size)
{
    void *virtual_addr = NULL;
    void *addr = kmalloc_ap(size, &virtual_addr);

    if (!addr)
        return (NULL);
    return (virtual_addr);
}

void *vfree(void *addr)
{
    kfree(get_physical_address(addr));
    return (NULL);
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
    return (ksize(get_physical_address(addr)));
}
