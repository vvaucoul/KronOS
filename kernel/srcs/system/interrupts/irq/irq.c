/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irq.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:56:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 11:28:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/irq.h>

/* External declarations for IRQ handlers */
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

const void *isq_fn[16] = {
	irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7,
	irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15};

/* Array of IRQ handlers */
static void (*irq_routines[16])(struct regs *r) = {0};

/**
 * Sends an End of Interrupt (EOI) signal to the PIC 8259 controller for the specified IRQ.
 *
 * @param irq The IRQ number to send EOI signal for.
 */
void pic8259_send_eoi(uint8_t irq) {
	if (irq >= 0x28) {
		outportb(SLAVE_PIC, IRQ_EOI); /* Send reset signal to slave */
	}
	outportb(MASTER_PIC, IRQ_EOI); /* Send reset signal to master */
}

/**
 * Check if an interrupt request (IRQ) is installed.
 *
 * @param irq The interrupt request number to check.
 * @return True if the IRQ is installed, false otherwise.
 */
bool irq_check_install(int irq) {
	if (irq < 0 || irq > 15) {
		return (false);
	}
	return (irq_routines[irq] != 0);
}

/**
 * Installs an interrupt handler for a specific IRQ.
 *
 * @param irq The IRQ number to install the handler for.
 * @param handler A pointer to the interrupt handler function.
 */
void irq_install_handler(int irq, void (*handler)(struct regs *r)) {
	if (irq >= 0 && irq < 16) {
		irq_routines[irq] = handler;
	}
}

/**
 * Uninstalls the handler for the specified IRQ.
 *
 * @param irq The IRQ number.
 */
void irq_uninstall_handler(int irq) {
	if (irq >= 0 && irq < 16) {
		irq_routines[irq] = 0;
	}
}

/**
 * @brief Remaps the IRQs.
 *
 * This function is responsible for remapping the IRQs to different interrupt vectors.
 * It ensures that the IRQs are properly handled by the system.
 */
static void irq_remap(void) {
	/* Save masks */
	uint8_t master_mask = inb(MASTER_DATA);
	uint8_t slave_mask = inb(SLAVE_DATA);

	/* Start initialization sequence */
	outportb(MASTER_PIC, ICW1_INIT | ICW1_ICW4);
	outportb(SLAVE_PIC, ICW1_INIT | ICW1_ICW4);

	/* Set vector offsets */
	outportb(MASTER_DATA, MASTER_OFFSET);
	outportb(SLAVE_DATA, SLAVE_OFFSET);

	/* Tell master PIC about slave */
	outportb(MASTER_DATA, ICW3_MASTER);
	outportb(SLAVE_DATA, ICW3_SLAVE);

	/* Set mode */
	outportb(MASTER_DATA, ICW4_8086);
	outportb(SLAVE_DATA, ICW4_8086);

	/* Restore masks */
	outportb(MASTER_DATA, master_mask);
	outportb(SLAVE_DATA, slave_mask);
}

/**
 * @brief Installs the interrupt service routine for handling IRQs.
 *
 * This function is responsible for installing the interrupt service routine (ISR)
 * that will handle the interrupt requests (IRQs) in the system. It sets up the
 * necessary data structures and registers the ISR with the interrupt controller.
 *
 * @note This function should be called during system initialization to enable
 *       handling of IRQs.
 */
void irq_install(void) {
	/* Remap the IRQs */
	irq_remap();

	/* Set the IDT entries for IRQs */
	for (int i = 0; i < 16; ++i) {
		idt_set_gate(32 + i, (uint32_t)(uintptr_t)isq_fn[i], IDT_SELECTOR, IDT_FLAG_GATE);
	}

	/* Clear the IRQ routines */
	memset(irq_routines, 0, sizeof(irq_routines));

	/* Enable IRQs */
	// outportb(MASTER_DATA, 0x0);
	// outportb(SLAVE_DATA, 0x0);
}

/**
 * @brief Handles the interrupt request (IRQ) and processes the interrupt.
 *
 * This function is responsible for handling the interrupt request (IRQ) and processing the interrupt.
 * It takes a pointer to a `struct regs` object as a parameter, which contains the register values at the time of the interrupt.
 * The function does not return a value.
 *
 * @param r A pointer to a `struct regs` object containing the register values at the time of the interrupt.
 */
void irq_handler(struct regs *r) {
	if (r->int_no >= 32 && r->int_no < 48) {
		uint8_t irq = r->int_no - 32;
		void (*handler)(struct regs *r) = irq_routines[irq];
		if (handler) {
			handler(r);
		}
		pic8259_send_eoi(irq);
	}
}