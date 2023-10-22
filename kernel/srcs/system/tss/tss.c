/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 18:39:54 by vvaucoul         ###   ########.fr       */
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
    tss_entry.ss0 = kss; // Kernel stack segment
    tss_entry.esp0 = kesp; // Kernel stack pointer
    tss_entry.cs = 0x0B;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
    tss_flush(&tss_entry);
}

void tss_set_stack_segment(uint32_t kss)
{
    tss_entry.ss0 = kss;
}

void tss_set_stack_pointer(uint32_t kesp)
{
    tss_entry.esp0 = kesp;
}