/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:56:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/pit.h>
#include <system/gdt.h>

GDTEntry *gdt = (GDTEntry *)__GDT_ADDR;
GDTPtr gp;

void gdt_add_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    (&gdt[index])->base_low = (base & 0xFFFF);
    (&gdt[index])->base_middle = (base >> 16) & 0xFF;
    (&gdt[index])->base_high = (base >> 24) & 0xFF;
    (&gdt[index])->limit_low = (limit & 0xFFFF);
    (&gdt[index])->granularity = (limit >> 16) & 0x0F;
    (&gdt[index])->granularity |= granularity & 0xF0;
    (&gdt[index])->access = access;
}

void gdt_install(void)
{
    if (__GDT_LOGS__)
    {
        printk("(0x00000800 + 0xC0000000): %p\n", (0x00000800 + 0xC0000000));
        printk("__GDT_ADDR HEXA: %u\n", __GDT_ADDR);
        printk("__GDT_ADDR PTR: %p\n", gdt);
    }

    gdt_add_entry(0, 0, 0, 0, 0);
    gdt_add_entry(1, 0, 0xFFFFFFFF, (uint8_t)(GDT_CODE_PL0), GDT_ENTRY_FLAG_BASE);
    gdt_add_entry(2, 0, 0xFFFFFFFF, (uint8_t)(GDT_DATA_PL0), GDT_ENTRY_FLAG_BASE);
    gdt_add_entry(3, 0, 0xFFFFFFFF, (uint8_t)(GDT_STACK_PL0), GDT_ENTRY_FLAG_BASE);
    gdt_add_entry(4, 0, 0xFFFFFFFF, (uint8_t)(GDT_CODE_PL3), GDT_ENTRY_FLAG_BASE);
    gdt_add_entry(5, 0, 0xFFFFFFFF, (uint8_t)(GDT_DATA_PL3), GDT_ENTRY_FLAG_BASE);
    gdt_add_entry(6, 0, 0xFFFFFFFF, (uint8_t)(GDT_STACK_PL3), GDT_ENTRY_FLAG_BASE);

    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(&gdt[0]) * __GDT_SIZE) - 1;
    gp.base = __GDT_ADDR;

    /* Check if GDT don't reach the limit */
    if (gp.limit > __GDT_LIMIT)
    {
        KERNO_ASSIGN_ERROR(__KERRNO_SECTOR_GDT, KERRNO_GDT_LIMIT);
        kernel_panic(__GDT_ERROR_LIMIT);
    }

    if (__GDT_LOGS__)
    {
        printk("__GDT: Limit: %u\n", gp.limit);
        printk("__GDT Base: %u\n", gp.base);
        printk("__GDT ADDR : %p\n", __GDT_ADDR);
    }

    /* Flush the GDT */
    gdt_flush((uint32_t)(&gp));
    if (__GDT_LOGS__)
        printk("Flush GDT SUCCESS !\n");
}

/*
  esp is the stack pointer, ebp is/was for a stack frame

  ESP : Top stack pointer
  EBP : Bottom stack pointer
*/

extern void print_gdt(void)
{
    printk("%8%% GDT Entry: " _GREEN "%p\n" _END, __GDT_ADDR);
    printk("%8%% GDT Base: " _GREEN "%p\n" _END, gp.base);
    printk("%8%% GDT Limit: " _GREEN "%u\n" _END, gp.limit);

    printk(_YELLOW "\n%8%% BASE LOW | BASE MIDDLE | BASE HIGH | LIMIT LOW | GRAN | ACCESS\n" _END);

    printk("%8%% 0x%x ", gdt[0].base_low);
    printk("   \t0x%x ", gdt[0].base_middle);
    printk("  \t\t0x%x ", gdt[0].base_high);
    printk("    \t0x%x ", gdt[0].limit_low);
    printk("    \t0x%x ", gdt[0].granularity);
    printk("   0x%x ", gdt[0].access);
    printk("\n");

    for (size_t i = 1; i < __GDT_SIZE; i++)
    {
        printk("%8%% 0x%x ", gdt[i].base_low);
        printk("   \t0x%x ", gdt[i].base_middle);
        printk("  \t\t0x%x ", gdt[i].base_high);
        printk("    \t0x%x ", gdt[i].limit_low);
        printk(" \t0x%x ", gdt[i].granularity);
        printk("  0x%x ", gdt[i].access);
        printk("\n");
    }
}

extern void gdt_test(void)
{
    uint32_t ebp;
    uint32_t esp;

    char tmp[13];
    bzero(tmp, 13);
    memcpy(tmp, "Hello World!", 12);
    printk("%8%% Add to stack PTR[13]: " _GREEN "'Hello World!'\n" _END);
    printk("%8%% TMP: " _GREEN "0x%u\n\n" _END, tmp);

    GET_EBP(ebp);
    GET_ESP(esp);
    uint32_t limit = esp;
    uint32_t i = 0;

    ebp = (uint32_t)tmp - 64;
    do
    {
        if (ebp == (uint32_t)tmp)
        {
            printk("\n%8%% PTR Found: " _GREEN "[EBP: 0x%u]\n" _END, ebp);
            printk("%8%% TMP " _GREEN "[PTR: 0x%u]\n\n" _END, (int32_t *)&tmp);
            printk("%8%% 0x%u[0]: " _GREEN "%c\n" _END, ebp, (char)(*(char *)ebp));
            printk("%8%% 0x%u: " _GREEN "%s\n\n" _END, ebp, ((char *)ebp));
            khexdump(ebp - 32, 80);
            return;
        }
        else if ((char)(*(char *)ebp) > 0)
        {
            printk(_CYAN "0x%u <==> 0x%u: " _END "%2%% %s: %s\n" _END, ebp, (char)(*(char *)ebp), ebp, tmp);
        }
        ebp += 1;
        ++i;
    } while (i < limit);
}

extern void print_stack(void)
{
    uint32_t esp;
    uint32_t ebp;

    GET_EBP(ebp);
    GET_ESP(esp);

    khexdump(esp - 128, 128);
}

#undef __GDT_ADDR
#undef __GDT_SIZE
#undef __GDT_LIMIT
#undef __GDT_ERROR_LIMIT
#undef __GDT_USER_LIMIT

#undef _GDT_KERNEL_CODE
#undef _GDT_KERNEL_DATA
#undef _GDT_KERNEL_STACK
#undef _GDT_USER_CODE
#undef _GDT_USER_DATA
#undef _GDT_USER_STACK