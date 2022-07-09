/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/09 12:46:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/pit.h>
#include <system/gdt.h>
#include <system/panic.h>

GDTEntry gdt[__GDT_SIZE] = {
    GDT_ENTRY(0x0, 0x0, 0x0, 0x0),
    GDT_ENTRY(0x0, 0xFFFFFFFF, (uint8_t)(GDT_CODE_PL0), 0xCF),  // kernel code segmentmake
    GDT_ENTRY(0x0, 0xFFFFFFFF, (uint8_t)(GDT_DATA_PL0), 0xCF),  // kernel data segment
    GDT_ENTRY(0x0, 0xFFFFFFFF, (uint8_t)(GDT_STACK_PL0), 0xCF), // Kernel stack segment
    GDT_ENTRY(0x0, 0xFFFFFFFF, (uint8_t)(GDT_CODE_PL3), 0xCF),  // user code segment
    GDT_ENTRY(0x0, 0xFFFFFFFF, (uint8_t)(GDT_DATA_PL3), 0xCF),  // user data segment
    GDT_ENTRY(0x0, 0xFFFFFFFF, (uint8_t)(GDT_STACK_PL3), 0xCF), // user stack segment
};

GDTPtr *gp = (GDTPtr *)__GDT_ADDR;

// void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint16_t access, uint8_t gran)
// {
//     gdt[num].base_low = (base & 0xFFFF);
//     gdt[num].base_middle = (base >> 16) & 0xFF;
//     gdt[num].base_high = (base >> 24) & 0xFF;
//     gdt[num].limit_low = (limit & 0xFFFF);
//     gdt[num].granularity = ((limit >> 16) & 0x0F);
//     gdt[num].granularity |= (gran & 0xF0);
//     gdt[num].access = access;
// }

void gdt_install(void)
{
    /* Setup the GDT pointer and limit */
    gp->limit = (sizeof(GDTEntry) * __GDT_SIZE) - 1;
    gp->base = gdt;

    if (gp->limit > __GDT_LIMIT)
        kernel_panic(__GDT_ERROR_LIMIT);

    // gdt_set_gate(0, 0x0, 0x0, 0x0, 0x0);
    // gdt_set_gate(1, 0x0, 0xFFFFFFFF, (uint16_t)GDT_CODE_PL0, 0xCF); // kernel code segment
    // gdt_set_gate(2, 0x0, 0xFFFFFFFF, (uint16_t)GDT_DATA_PL0, 0xCF); // kernel data segment
    // gdt_set_gate(3, 0x0, 0xFFFFFFFF, (uint16_t)GDT_STACK_PL0, 0xCF); // Kernel stack segment

    // gdt_set_gate(4, 0x0, 0xFFFFFFFF, (uint16_t)GDT_CODE_PL3, 0xCF); // user code segment
    // gdt_set_gate(5, 0x0, 0xFFFFFFFF, (uint16_t)GDT_DATA_PL3, 0xCF); // user data segment
    // gdt_set_gate(6, 0x0, 0xFFFFFFFF, (uint16_t)GDT_STACK_PL3, 0xCF); // user stack segment

    // gdt_set_gate(7, &tss, sizeof(tss), 0x89, 0x0);

    /* Flush the GDT */
    gdt_flush((uint32_t)gp);
}

/*
  esp is the stack pointer, ebp is/was for a stack frame

  ESP : Top stack pointer
  EBP : Bottom stack pointer
*/

extern void print_gdt(void)
{
    kprintf("%8%% GDT Entry: 0x00000%x\n", __GDT_ADDR);
    kprintf("%8%% GDT Base: 0x0%x\n", gp->base);
    kprintf("%8%% GDT Limit: 0x00000%x\n", gp->limit);

    kprintf("\n%8%% BASE LOW | BASE MIDDLE | BASE HIGH | LIMIT LOW | GRAN | ACCESS\n");

    kprintf("%8%% 0x%x ", gdt[0].base_low);
    kprintf("   \t0x%x ", gdt[0].base_middle);
    kprintf("  \t\t0x%x ", gdt[0].base_high);
    kprintf("    \t0x%x ", gdt[0].limit_low);
    kprintf("    \t0x%x ", gdt[0].granularity);
    kprintf("   0x%x ", gdt[0].access);
    kprintf("\n");

    for (size_t i = 1; i < __GDT_SIZE; i++)
    {
        kprintf("%8%% 0x%x ", gdt[i].base_low);
        kprintf("   \t0x%x ", gdt[i].base_middle);
        kprintf("  \t\t0x%x ", gdt[i].base_high);
        kprintf("    \t0x%x ", gdt[i].limit_low);
        kprintf(" \t0x%x ", gdt[i].granularity);
        kprintf("  0x%x ", gdt[i].access);
        kprintf("\n");
    }
}

extern void gdt_test(void)
{
    char tmp[128];
    int32_t ebp;
    int32_t esp;

    kbzero(tmp, 128);
    tmp[0] = 'a';
    kprintf("%8%% Add to stack PTR[128] = 'a'\n");
    kprintf("%8%% TMP: 0x%x\n\n", tmp);

    GET_EBP(ebp);
    GET_ESP(esp);
    int limit = esp;
    int i = 0;

    kprintf("%8%% LIMIT: %d\n", limit);
    kprintf("%8%% PTR: 0x%x\n", &tmp);
    ksleep(1);
    do
    {
        if (ebp == (int32_t)tmp )//|| (char)(*(char *)ebp) == 'a')
        {
            kprintf("%8%% PTR Found: [EBP: 0x%x]\n", ebp);
            kprintf("%8%% TMP [PTR: 0x%x]\n\n", (int32_t *)&tmp);
            kprintf("%8%% 0x%x: %c\n", ebp, (char)(*(char *)ebp));
            return;
        }
        else if ((char)(*(char *)ebp) > 0)
        {
            kprintf("%8%% 0x%x: %c\tcheck: %x <==> %x\n", ebp, (char)(*(char *)ebp), ebp, tmp);
        }
        ebp += 1;
        ++i;
    } while (i < limit);
}

extern void print_stack(void)
{
    int32_t ebp;
    int32_t esp;

    GET_EBP(ebp);
    GET_ESP(esp);

    int limit = esp - ebp;
    kprintf("Limit: %d\n", limit);
    ksleep(2);
    // return ;
    for (int i = 0; i < limit; i++)
    {
        // if ((void *)esp == (void *)(&tmp))
        //     return;
        if (i % 16 == 0 && *(char *)esp != 0)
        {
            kprintf("%8%% 0x%x\t\t%c\n", esp, (char)(*(char *)esp));
            esp += 16;
        }
    }
    ksleep(2);
    kprintf("%8%% 0x%x\t\t0x0%s\n", esp, (char *)esp);

    kprintf("ESP (START): 0x%x\n", esp);
    kprintf("EBP (CURRENT): 0x%x\n", ebp);
    kprintf("EBP C: %c\n", *(char *)ebp);
    // kprintf("%8%% TMP ptr: 0x%x\n\n", &tmp);

    ksleep(2);
    int i = 0;
    do
    {
        // if (&ebp == (int32_t *)&tmp || (char)(*(char *)ebp) == 'a')
        // {
        //     kprintf("%8%% TMP ptr: 0x%x\n\n", (int32_t *)&tmp);
        //     kprintf("%8%% 0x%x: %c\n", ebp, (char)(*(char *)ebp));
        //     kprintf("Holla !\n");
        //     return;
        // }
        if ((char)(*(char *)ebp) != 0 && (char)(*(char *)ebp) > 0)
            kprintf("%8%% 0x%x: %c\n", ebp, (char)(*(char *)ebp));
        ebp += 1;
        ++i;
    } while (i < limit);
    ksleep(2);
    // kprintf("%8%% 0x%x: %c\n", ebp, (char)(*(char *)ebp));
    // kprintf("%8%% TMP ptr: 0x%x\n\n", &tmp);
}

#undef __GDT_ADDR
#undef __GDT_SIZE
#undef __GDT_LIMIT
#undef __GDT_ERROR_LIMIT