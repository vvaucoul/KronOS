/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 19:41:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/system/tss.h"
#include "../../../includes/system/gdt.h"

void tss_load(unsigned long cpu_num)
{
    (void)cpu_num;
    // unsigned long tss_addr;

    // tss_addr = (unsigned long)tss_table + cpu_num;

    // /* build descriptor */
    // gdt[5 + cpu_num] =
    //     /* base */
    //     ((unsigned long long)(tss_addr & 0x00ffffff) << 16) + ((unsigned long long)(tss_addr & 0xff000000) << 32)
    //     /* attributes, 32-bit TSS, present,  */
    //     + 0x0000890000000000LL
    //     /* limit, it's less than 2^16 anyhow, so no need for th eupper nibble */
    //     + (sizeof(tss)) - 1;

    // asm volatile("ltr %%ax"
    //              :
    //              : "a"((5 + cpu_num) << 3));
}