/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 21:59:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/gdt.h>
#include <system/pit.h>
#include <system/tss.h>

GDTEntry *gdt = (GDTEntry *)GDT_ADDRESS;
GDTPtr gp;

void gdt_add_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = (limit >> 16) & 0x0F;
    gdt[index].granularity |= granularity & 0xF0;
    gdt[index].access = access;
}

void gdt_init(void) {
    gp.limit = (sizeof(GDTEntry) * (GDT_SIZE + TSS_SIZE)) - 1;
    gp.base = GDT_ADDRESS;

    gdt_add_entry(0, 0, 0, 0, 0);
    gdt_add_entry(1, 0, 0xFFFFFFFF, (uint8_t)(GDT_CODE_PL0), GDT_ENTRY_FLAG_BASE);  // Kernel code segment (PL0) - 0x9A - 0xCF
    gdt_add_entry(2, 0, 0xFFFFFFFF, (uint8_t)(GDT_DATA_PL0), GDT_ENTRY_FLAG_BASE);  // Kernel data segment (PL0) - 0x92 - 0xCF
    gdt_add_entry(3, 0, 0xFFFFFFFF, (uint8_t)(GDT_STACK_PL0), GDT_ENTRY_FLAG_BASE); // Kernel stack segment (PL0) - 0x92 - 0xCF
    gdt_add_entry(4, 0, 0xFFFFFFFF, (uint8_t)(GDT_CODE_PL3), GDT_ENTRY_FLAG_BASE);  // User code segment (PL3) - 0xFA - 0xCF
    gdt_add_entry(5, 0, 0xFFFFFFFF, (uint8_t)(GDT_DATA_PL3), GDT_ENTRY_FLAG_BASE);  // User data segment (PL3) - 0xF2 - 0xCF
    gdt_add_entry(6, 0, 0xFFFFFFFF, (uint8_t)(GDT_STACK_PL3), GDT_ENTRY_FLAG_BASE); // User stack segment (PL3) - 0xF2 - 0xCF

    gdt_flush((uint32_t)(uintptr_t)&gp);
}