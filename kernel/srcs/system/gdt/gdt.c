/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/01 11:31:17 by vvaucoul         ###   ########.fr       */
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
    kprintf("%8%% GDT Entry: 0x00000%x\n", __GDT_ADDR__);
    kprintf("%8%% GDT Base: 0x0%x\n", gp->base);
    kprintf("%8%% GDT Limit: 0x00000%x\n", gp->limit);

    kprintf("\n%8%% BASE LOW | BASE MIDDLE | BASE HIGH | LIMIT LOW | GRAN | ACCESS\n");

    kprintf("%8%% 0x0%x ", gdt[0].base_low);
    kprintf("  \t0x0%x ", gdt[0].base_middle);
    kprintf(" \t\t0x0%x ", gdt[0].base_high);
    kprintf("   \t0x0%x ", gdt[0].limit_low);
    kprintf("   \t0x0%x ", gdt[0].granularity);
    kprintf("  0x0%x ", gdt[0].access);
    kprintf("\n");

    for (size_t i = 1; i < GDT_SIZE; i++)
    {
        kprintf("%8%% 0x0%x ", gdt[i].base_low);
        kprintf("  \t0x0%x ", gdt[i].base_middle);
        kprintf(" \t\t0x0%x ", gdt[i].base_high);
        kprintf("   \t0x0%x ", gdt[i].limit_low);
        kprintf("\t0x0%x ", gdt[i].granularity);
        kprintf(" 0x0%x ", gdt[i].access);
        kprintf("\n");
    }

    int32_t ebp;
    int32_t esp;

    char tmp[128];

    kbzero(tmp, 128);
    tmp[0] = 'a';
    kprintf("\n%8%% Add to stack PTR[128]\n");

    kprintf("%8%% Stack: \n");

    GET_EBP(ebp);
    GET_ESP(esp);
    while (ebp != 0)
    {
        kprintf("%8%% 0x%x\n", ebp);
        ebp = *(int32_t *)ebp;
    }
    kprintf("%8%% 0x%x\n", esp);
}

#undef __GDT_ADDR__