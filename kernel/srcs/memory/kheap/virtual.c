/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:49:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/27 17:39:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

void *vmalloc(uint32_t size) {
    return kmalloc_v(size);
}

void vfree(void *addr) {
    kfree_v(addr);
}

void *vrealloc(void *addr, uint32_t size) {
    void *new_addr = vmalloc(size);
    if (!new_addr) return NULL;
    memcpy(new_addr, addr, size);
    kfree(addr);
    return new_addr;
}

void *vcalloc(uint32_t count, uint32_t size) {
    void *addr = vmalloc(count * size);
    if (!addr) return NULL;
    memset(addr, 0, size);
    return addr;
}

uint32_t vsize(void *addr) {
    return ksize(addr);
}

void *vbrk(uint32_t size) {
    return kbrk(size);
}