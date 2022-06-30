/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 15:12:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/system/gdt.h"
#include "../../../includes/system/tss.h"

GDT_ENTRY gdt[GDT_SIZE];
GDT_PTR *gp = (GDT_PTR *)__GDT_ADDR__;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint16_t access, uint8_t gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install(void)
{
    /* Setup the GDT pointer and limit */
    gp->limit = (sizeof(GDT_ENTRY) * GDT_SIZE) - 1;
    gp->base = ((uint32_t)&gdt);

    gdt_set_gate(0, 0x0, 0x0, 0x0, 0x0);
    gdt_set_gate(1, 0x0, 0xFFFFFFFF, (uint16_t)GDT_CODE_PL0, 0xCF); // kernel code segment
    gdt_set_gate(2, 0x0, 0xFFFFFFFF, (uint16_t)GDT_DATA_PL0, 0xCF); // kernel data segment
    gdt_set_gate(3, 0x0, 0xFFFFFFFF, (uint16_t)GDT_STACK_PL0, 0xCF); // Kernel stack segment

    gdt_set_gate(4, 0x0, 0xFFFFFFFF, (uint16_t)GDT_CODE_PL3, 0xCF); // user code segment
    gdt_set_gate(5, 0x0, 0xFFFFFFFF, (uint16_t)GDT_DATA_PL3, 0xCF); // user data segment
    gdt_set_gate(6, 0x0, 0xFFFFFFFF, (uint16_t)GDT_STACK_PL3, 0xCF); // user stack segment

    // gdt_set_gate(7, &tss, sizeof(tss), 0x89, 0x0);

    /* Flush the GDT */
    gdt_flush((uint32_t)gp);
}

extern void print_stack(void)
{
    int32_t ebp;
    int32_t esp;

    GET_EBP(ebp);
    GET_ESP(esp);

    kprintf("EBP: %x\n", ebp);
    kprintf("ESP: %x\n", esp);
    

    kprintf("Test 01\n");
    kprintf("Test 02\n");
    kprintf("Test 03\n");
    kprintf("Test 04\n");
    kprintf("Test 05\n");
}

#undef __GDT_ADDR__