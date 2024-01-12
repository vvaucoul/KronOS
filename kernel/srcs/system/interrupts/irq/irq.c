/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irq.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:56:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 20:24:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/irq.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void *irq_routines[16] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0};

void pic8259_send_eoi(uint8_t irq) {
    if (irq >= 0x28) {
        /* Send reset signal to slave. */
        outportb(SLAVE_PIC, IRQ_EOI);
    }
    /* Send reset signal to master. (As well as slave, if necessary). */
    outportb(MASTER_PIC, IRQ_EOI);
}

bool irq_check_install(int irq) {
    return (irq_routines[irq] != 0);
}

void irq_install_handler(int irq, void (*handler)(struct regs *r)) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

void irq_remap(void) {
    /* Maybe remap to setup cascading */

    uint32_t master_mask = inb(MASTER_DATA);
    uint32_t slave_mask = inb(SLAVE_DATA);

    outportb(MASTER_PIC, ICW1_INIT | ICW1_ICW4);
    outportb(SLAVE_PIC, ICW1_INIT | ICW1_ICW4);

    outportb(MASTER_DATA, MASTER_OFFSET);
    outportb(SLAVE_DATA, SLAVE_OFFSET);

    outportb(MASTER_DATA, ICW3_MASTER);
    outportb(SLAVE_DATA, ICW3_SLAVE);

    outportb(MASTER_DATA, ICW4_8086);
    outportb(SLAVE_DATA, ICW4_8086);

    outportb(MASTER_DATA, master_mask);
    outportb(SLAVE_DATA, slave_mask);
}

void irq_install() {
    irq_remap();

    idt_set_gate(32, (unsigned)irq0, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(33, (unsigned)irq1, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(34, (unsigned)irq2, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(35, (unsigned)irq3, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(36, (unsigned)irq4, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(37, (unsigned)irq5, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(38, (unsigned)irq6, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(39, (unsigned)irq7, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(40, (unsigned)irq8, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(41, (unsigned)irq9, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(42, (unsigned)irq10, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(43, (unsigned)irq11, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(44, (unsigned)irq12, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(45, (unsigned)irq13, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(46, (unsigned)irq14, IDT_SELECTOR, IDT_FLAG_GATE);
    idt_set_gate(47, (unsigned)irq15, IDT_SELECTOR, IDT_FLAG_GATE);
}

void irq_handler(struct regs *r) {
    void (*handler)(struct regs *r);

    handler = irq_routines[r->int_no - 32];
    if (handler) {
        /* Call the handler. */
        if (handler)
            handler(r);
    }
    pic8259_send_eoi(r->int_no);
}