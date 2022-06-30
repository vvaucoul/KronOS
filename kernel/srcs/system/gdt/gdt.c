/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 13:56:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/system/gdt.h"
#include "../../../includes/system/tss.h"

// #include <gdt.h>

struct gdt_entry gdt[GDT_SIZE];
struct gdt_ptr gp;

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

/* Should be called by main. This will setup the special GDT
 *  pointer, set up the first 3 entries in our GDT, and then
 *  finally call gdt_flush() in our assembler file in order
 *  to tell the processor where the new GDT is and update the
 *  new segment registers */
void gdt_install(void)
{
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * GDT_SIZE) - 1;
    gp.base = (unsigned int)&gdt;

    gdt_set_gate(0, 0x0, 0x0, 0x0, 0x0);
    gdt_set_gate(1, 0x0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0x0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0x0, 0xFFFFFFFF, 0xFA, 0xC);
    gdt_set_gate(4, 0x0, 0xFFFFFFFF, 0xF2, 0xC);
    
    // gdt_set_gate(5, &tss, sizeof(tss), 0x89, 0x0);

    gdt_flush();
}

extern void print_stack(void)
{
    kprintf("Stack: %x\n", gdt->limit_low);

    kprintf("Test 01\n");
    kprintf("Test 02\n");
    kprintf("Test 03\n");
    kprintf("Test 04\n");
    kprintf("Test 05\n");
}