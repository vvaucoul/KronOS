/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:09:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 17:51:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/idt.h>
#include <system/isr.h>

struct idt_entry idt[IDT_SIZE] __attribute__((aligned(0x10)));
struct idt_ptr idtp;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags)
{
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector = selector; // Selector
    idt[num].zero = 0;
    idt[num].flags = flags;// | 0x60;// <- Uncomment this for user mode interrupts
}

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    idtp.base = (unsigned int)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * IDT_SIZE);

    /* Init Interrupt Handlers */
    memset(&g_interrupt_handlers, 0, sizeof(ISR) * NB_INTERRUPT_HANDLERS);

    /* Load IDT */
    idt_load(&idtp);
}

static inline void read_idtr(struct idt_ptr* idtr) {
    __asm__ volatile ("sidt (%0)" : : "r"(idtr));
}

void print_idt_entry(uint8_t num) {
    struct idt_ptr idtr;
    read_idtr(&idtr);

    struct idt_entry* idt = (struct idt_entry*)idtr.base;
    struct idt_entry entry = idt[num];

    printk("IDT Entry %d:\n", num);
    printk("Base: 0x%04x%04x\n", entry.base_high, entry.base_low);
    printk("Selector: 0x%04x\n", entry.selector);
    printk("Flags: 0x%02x\n", entry.flags);
}