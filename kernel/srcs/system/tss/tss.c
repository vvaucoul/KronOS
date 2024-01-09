/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:20:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/gdt.h>
#include <system/tss.h>

tss_entry_t tss_entry;

void tss_init(uint32_t idx, uint32_t kss, uint32_t kesp) {
    gdt_add_entry(idx, (uint32_t)(&tss_entry), ((uint32_t)(&tss_entry)) + sizeof(tss_entry_t), TSS_KERNEL_ACCESS, 0x0);
    gdt_add_entry(idx + 1, 0x0, 0x0, 0x0, 0x0);

    memset((uint32_t *)&tss_entry, 0, sizeof(tss_entry_t));

    tss_entry.ss0 = kss;   // Kernel stack segment
    tss_entry.esp0 = kesp; // Kernel stack pointer
    tss_entry.cs = 0x0B;  // Code segment
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
    tss_flush(&tss_entry);
}

/**
 * Set the kernel stack segment
 * @param kss Kernel stack segment
 */
void tss_set_stack_segment(uint32_t kss) {
    tss_entry.ss0 = kss;
}

/**
 * Set the kernel stack pointer
 * @param kesp Kernel stack pointer
 */
void tss_set_stack_pointer(uint32_t kesp) {
    tss_entry.esp0 = kesp;
}