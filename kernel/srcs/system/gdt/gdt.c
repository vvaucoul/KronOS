/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/17 17:51:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/pit.h>
#include <system/gdt.h>
#include <system/panic.h>
#include <system/kerrno.h>

GDTEntry gdt[__GDT_SIZE] = {
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, GDT_ENTRY_FLAG_ZERO, GDT_ENTRY_FLAG_ZERO, GDT_ENTRY_FLAG_ZERO),
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, __GDT_LIMIT, (uint8_t)(GDT_CODE_PL0), GDT_ENTRY_FLAG_BASE),       // kernel code segment
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, __GDT_LIMIT, (uint8_t)(GDT_DATA_PL0), GDT_ENTRY_FLAG_BASE),       // kernel data segment
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, __GDT_LIMIT, (uint8_t)(GDT_STACK_PL0), GDT_ENTRY_FLAG_BASE),      // Kernel stack segment
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, __GDT_USER_LIMIT, (uint8_t)(GDT_CODE_PL3), GDT_ENTRY_FLAG_BASE),  // user code segment
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, __GDT_USER_LIMIT, (uint8_t)(GDT_DATA_PL3), GDT_ENTRY_FLAG_BASE),  // user data segment
    GDT_ENTRY(GDT_ENTRY_FLAG_ZERO, __GDT_USER_LIMIT, (uint8_t)(GDT_STACK_PL3), GDT_ENTRY_FLAG_BASE), // user stack segment
};

GDTPtr *gp = (GDTPtr *)__GDT_ADDR;

void gdt_install(void)
{
    /* Setup the GDT pointer and limit */
    gp->limit = (sizeof(GDTEntry) * __GDT_SIZE) - 1;
    gp->base = ((uint32_t)(&gdt));

    /* Check if GDT don't reach the limit */
    if (gp->limit > __GDT_LIMIT)
    {
        kerrno_assign_error(__KERRNO_SECTOR_GDT, KERRNO_GDT_LIMIT, __FILE_NAME__, __FUNCTION__);
        kernel_panic(__GDT_ERROR_LIMIT);
    }

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
    kprintf("%8%% GDT Entry: " COLOR_GREEN "0x00000%x\n" COLOR_END, __GDT_ADDR);
    kprintf("%8%% GDT Base: " COLOR_GREEN "0x0%x\n" COLOR_END, gp->base);
    kprintf("%8%% GDT Limit: " COLOR_GREEN "%d\n" COLOR_END, gp->limit);

    kprintf(COLOR_YELLOW "\n%8%% BASE LOW | BASE MIDDLE | BASE HIGH | LIMIT LOW | GRAN | ACCESS\n" COLOR_END);

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
    uint32_t ebp;
    uint32_t esp;

    char tmp[13];
    kbzero(tmp, 13);
    kmemcpy(tmp, "Hello World!", 12);
    kprintf("%8%% Add to stack PTR[13]: " COLOR_GREEN "'Hello World!'\n" COLOR_END);
    kprintf("%8%% TMP: " COLOR_GREEN "0x%x\n\n" COLOR_END, tmp);

    GET_EBP(ebp);
    GET_ESP(esp);
    int limit = esp;
    int i = 0;

    ksleep(2);
    ebp = (uint32_t)tmp - 64;
    do
    {
        if (ebp == (uint32_t)tmp)
        {
            kprintf("\n%8%% PTR Found: " COLOR_GREEN "[EBP: 0x%x]\n" COLOR_END, ebp);
            kprintf("%8%% TMP " COLOR_GREEN "[PTR: 0x%x]\n\n" COLOR_END, (int32_t *)&tmp);
            kprintf("%8%% 0x%x[0]: " COLOR_GREEN "%c\n" COLOR_END, ebp, (char)(*(char *)ebp));
            kprintf("%8%% 0x%x: " COLOR_GREEN "%s\n\n" COLOR_END, ebp, ((char *)ebp));
            khexdump(ebp - 32, 80);
            return;
        }
        else if ((char)(*(char *)ebp) > 0)
        {
            kprintf(COLOR_CYAN "0x%x <==> 0x%x: " COLOR_END "%2%% %s: %s\n" COLOR_END, ebp, (char)(*(char *)ebp), ebp, tmp);
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