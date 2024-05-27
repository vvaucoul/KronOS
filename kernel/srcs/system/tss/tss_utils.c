/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 12:41:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 12:42:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/tss.h>

void print_tss(void) {
    extern tss_entry_t tss_entry;
    printk("+----------------+------------+\n");
    printk("| Field          | Value      |\n");
    printk("+----------------+------------+\n");
    printk("| ESP0           | %10x |\n", tss_entry.esp0);
    printk("| SS0            | %10x |\n", tss_entry.ss0);
    printk("| ESP1           | %10x |\n", tss_entry.esp1);
    printk("| SS1            | %10x |\n", tss_entry.ss1);
    printk("| ESP2           | %10x |\n", tss_entry.esp2);
    printk("| SS2            | %10x |\n", tss_entry.ss2);
    printk("| CR3            | %10x |\n", tss_entry.cr3);
    printk("| EIP            | %10x |\n", tss_entry.eip);
    printk("| EFLAGS         | %10x |\n", tss_entry.eflags);
    printk("| EAX            | %10x |\n", tss_entry.eax);
    printk("| ECX            | %10x |\n", tss_entry.ecx);
    printk("| EDX            | %10x |\n", tss_entry.edx);
    printk("| EBX            | %10x |\n", tss_entry.ebx);
    printk("| ESP            | %10x |\n", tss_entry.esp);
    printk("| EBP            | %10x |\n", tss_entry.ebp);
    printk("| ESI            | %10x |\n", tss_entry.esi);
    printk("| EDI            | %10x |\n", tss_entry.edi);
    printk("| ES             | %10x |\n", tss_entry.es);
    printk("| CS             | %10x |\n", tss_entry.cs);
    printk("| SS             | %10x |\n", tss_entry.ss);
    printk("| DS             | %10x |\n", tss_entry.ds);
    printk("| FS             | %10x |\n", tss_entry.fs);
    printk("| GS             | %10x |\n", tss_entry.gs);
    printk("| LDT            | %10x |\n", tss_entry.ldt);
    printk("| Trap           | %10x |\n", tss_entry.trap);
    printk("| IOMAP          | %10x |\n", tss_entry.iomap);
    printk("+----------------+------------+\n");
}

static inline void read_gdt(void *gdt_ptr) {
    __asm__ volatile("sgdt (%0)" : : "r"(gdt_ptr));
}

static inline uint16_t read_tr(void) {
    uint16_t tr;
    __asm__ volatile("str %0" : "=r"(tr));
    return tr;
}

void check_gdt_tss(void) {
    uint8_t gdt_info[10];
    read_gdt(gdt_info);

    uint16_t tr = read_tr();
    printk("GDTR: Limit %u, Base %x\n", *(uint16_t *)&gdt_info[0], *(uint32_t *)&gdt_info[2]);
    printk("TR: %x\n", tr);
}