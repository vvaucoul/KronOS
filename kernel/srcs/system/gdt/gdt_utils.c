/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 12:35:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 12:39:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/gdt.h>
#include <system/tss.h>

void print_gdt(void) {
    extern GDTPtr gp;
    extern GDTEntry *gdt;
    printk("GDT Base: 0x%x\n", gp.base);
    printk("GDT Limit: %u\n", gp.limit);
    printk("+-------+------------+-------+--------+-------------+\n");
    printk("| Index | Base       | Limit | Access | Granularity |\n");
    printk("+-------+------------+-------+--------+-------------+\n");

    for (int i = 0; i < (GDT_SIZE + TSS_SIZE); ++i) {
        printk("| %5d | 0x%08x | %5x | %6x | %11x |\n",
               i, (gdt[i].base_low | (gdt[i].base_middle << 16) | (gdt[i].base_high << 24)),
               (gdt[i].limit_low | ((gdt[i].granularity & 0x0F) << 16)),
               gdt[i].access, gdt[i].granularity);
    }
    printk("+-------+------------+-------+--------+-------------+\n");
}