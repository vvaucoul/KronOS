/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 12:10:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/tss.h>
#include <system/gdt.h>

tss_entry_t tss_entry;

void tss_init(uint32_t idx, uint32_t kss, uint32_t kesp)
{
    uint32_t base_addr = (uint32_t)&tss_entry;

    gdt_add_entry(idx, base_addr, base_addr + sizeof(tss_entry_t), 0xE9, 0x00);
    memset((uint32_t *)&tss_entry, 0, sizeof(tss_entry_t));
    tss_entry.ss0 = kss;
    tss_entry.esp0 = kesp;
    tss_entry.cs = 0x0b;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
    tss_flush(&tss_entry);
}

void tss_set_stack(uint32_t kss, uint32_t kesp)
{
    tss_entry.ss0 = kss;
    tss_entry.esp0 = kesp;
}
