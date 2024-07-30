/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:09:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 01:49:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/idt.h>
#include <system/isr.h>

static struct idt_entry idt[IDT_ENTRIES] = {0};
static struct idt_ptr idtp;

/**
 * Sets a gate in the Interrupt Descriptor Table (IDT).
 *
 * @param num      The interrupt number.
 * @param base     The base address of the interrupt handler function.
 * @param selector The selector of the interrupt handler function.
 * @param flags    The flags for the interrupt gate.
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
	idt[num].base_low = (base & 0xFFFF);
	idt[num].base_high = (base >> 16) & 0xFFFF;
	idt[num].selector = selector;  /* Kernel segment selector */
	idt[num].zero = 0;			   /* Must always be zero */
	idt[num].flags = flags | 0x60; /* Flags, e.g., 0x8E for interrupt gate */
								   // | 0x60;// <- this is for user mode interrupts
}

/**
 * @brief Installs the Interrupt Descriptor Table (IDT).
 *
 * This function is responsible for installing the Interrupt Descriptor Table (IDT)
 * in the system. The IDT is a data structure used by the processor to handle interrupts
 * and exceptions. By installing the IDT, the system can properly handle hardware and
 * software interrupts.
 */
void idt_install() {
	/* Sets the special IDT pointer up, just like in 'gdt.c' */
	idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
	idtp.base = (uint32_t)&idt;

	/* Load IDT */
	idt_load(&idtp);
}

/**
 * Reads the Interrupt Descriptor Table Register (IDTR) value.
 *
 * @param idtr Pointer to the IDT pointer structure.
 */
static inline void read_idtr(struct idt_ptr *idtr) {
	__asm__ volatile("sidt (%0)" : : "r"(idtr));
}
