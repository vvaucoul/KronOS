/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:09:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 17:04:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/idt.h>
#include <system/isr.h>
#include <asm/asm.h>

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtp;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags)
{
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags; //| 0x60; // <- Uncomment this for user mode interrupts
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