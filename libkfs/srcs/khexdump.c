/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   khexdump.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/11 13:29:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 02:44:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libkfs.h"

void khexdump(uint32_t ebp, int limit)
{
    int i = 0;

    if (limit <= 0)
        return;
    do
    {
        kprintf(ebp == 0x00000800 ? COLOR_GREEN : COLOR_CYAN);
        kprintf("0x%u: " COLOR_END, ebp);

        uint32_t next = ebp + 16;
        uint32_t tmp = ebp;
        while (tmp < next && i < limit)
        {
            if (*(char *)tmp >= 32)
                kprintf(COLOR_GREEN "%u " COLOR_END, *(char *)tmp);
            else
                kprintf(COLOR_END "00 " COLOR_END);
            ++tmp;
        }

        next = ebp + 16;
        tmp = ebp;
        while (tmp < next && i < limit)
        {
            if (*(char *)tmp > 32)
                kprintf("%c", *(char *)tmp);
            else
                kprintf(".");
            ++tmp;
        }
        kprintf("\n");
        i += 16;
        ebp += 16;
    } while (i < limit);
}