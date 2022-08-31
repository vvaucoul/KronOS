/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/30 17:55:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/30 18:08:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

void *physical_to_virtual_memory(void *virtualaddr)
{
    // unsigned long pdindex = (unsigned long) virtualaddr >> 22;
    // unsigned long ptindex = (unsigned long) virtualaddr >> 12 & 0x03FF;

    // unsigned long *pd = (unsigned long *)0xFFFFF000;
    // unsigned long *pt = ((unsigned long *)0xFFC0000) + (0x400 * ptindex);
    // return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}

void virtual_to_physical_memory(void *physaddr, void *virtualaddr, unsigned int flags)
{
    // unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    // unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;

    // unsigned long *pd = (unsigned long *)0xFFFFF000;
    // unsigned long *pt = ((unsigned long *)°0xFFC00000) + (0x400 * pdindex);
    // pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01;
}