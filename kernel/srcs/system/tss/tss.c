/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 23:00:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/gdt.h>
#include <system/tss.h>

tss_entry_t tss_entry;

void tss_init(uint32_t idx, uint32_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)(&tss_entry);
    uint32_t limit = base + sizeof(tss_entry_t);

    gdt_add_entry(idx, base, limit, TSS_KERNEL_ACCESS, 0x0);

    memset(&tss_entry, 0, sizeof(tss_entry_t));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x1B;                                                             // User mode code segment selector
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x23; // User mode data segment selectors
    tss_entry.iomap = sizeof(tss_entry_t);

    gdt_flush((uint32_t)((uint32_t)(&gp)));
    tss_flush();
}

/**
 * Set the kernel stack segment
 * @param ss0 Kernel stack segment
 */
void tss_set_stack_segment(uint32_t ss0) {
    tss_entry.ss0 = ss0;
}

/**
 * Set the kernel stack pointer
 * @param esp0 Kernel stack pointer
 */
void tss_set_stack_pointer(uint32_t esp0) {
    tss_entry.esp0 = esp0;
}

/**
 * Sets the stack values for the Task State Segment (TSS).
 *
 * @param ss0 The kernel stack segment selector.
 * @param esp0 The kernel stack pointer.
 */
void tss_set_stack(uint32_t ss0, uint32_t esp0) {
    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
}