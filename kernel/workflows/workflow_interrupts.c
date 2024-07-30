/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_interrupts.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 21:57:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 01:49:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <system/idt.h>
#include <system/irq.h>
#include <system/isr.h>
#include <system/panic.h>
#include <workflows/workflows.h>

#include <assert.h>
#include <system/pit.h>

// Todo: Add tests for the IDT

// void test_idt_set_gate() ;
// void test_idt_install() ;
// void test_read_idtr() ;

/* TMP TESTS */

// #include <assert.h>

// void test_idt_set_gate() {
//     uint8_t num = 1;
//     uint32_t base = 0x12345678;
//     uint16_t selector = 0x08;
//     uint8_t flags = 0x8E;

//     idt_set_gate(num, base, selector, flags);

//     assert(idt[num].base_low == (base & 0xFFFF));
//     assert(idt[num].base_high == ((base >> 16) & 0xFFFF));
//     assert(idt[num].selector == selector);
//     assert(idt[num].zero == 0);
//     assert(idt[num].flags == (flags | 0x60));
// }

// void test_idt_install() {
//     idt_install();

//     assert(idtp.limit == (sizeof(struct idt_entry) * IDT_ENTRIES) - 1);
//     assert(idtp.base == (uint32_t)&idt);
// }

// void test_read_idtr() {
//     struct idt_ptr idtr;
//     idtr.limit = 0xFFFF;
//     idtr.base = (uint32_t)&idt;

//     read_idtr(&idtr);

//     // Assuming the mock sidt sets the idtr correctly
//     assert(idtr.limit == 0xFFFF);
//     assert(idtr.base == (uint32_t)&idt);
// }

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void interrupts_test(void) {
	/* FAULT - INTERRUPTS - TRAPS */

	/* Division by zero */
	__asm__ volatile("int $0x0");
	ksleep(1);

	/* Debug */
	__asm__ volatile("int $0x1");
	ksleep(1);

	/* Non Maskable Interrupt */
	__asm__ volatile("int $0x2");
	ksleep(1);

	/* Breakpoint */
	__asm__ volatile("int $0x3");
	ksleep(1);

	/* Overflow */
	__asm__ volatile("int $0x4");
	ksleep(1);

	/* Bound Range Exceeded */
	__asm__ volatile("int $0x5");
	ksleep(1);

	/* Invalid Opcode */
	__asm__ volatile("int $0x6");
	ksleep(1);

	/* Device Not Available */
	__asm__ volatile("int $0x7");
	ksleep(1);

	/* Coprocessor Segment Overrun */
	__asm__ volatile("int $0x9");
	ksleep(1);

	/* Double Fault */
	__asm__ volatile("int $0x8");
	ksleep(1);

	/* Page Fault */
	__asm__ volatile("int $0x0e");
	ksleep(1);
}
