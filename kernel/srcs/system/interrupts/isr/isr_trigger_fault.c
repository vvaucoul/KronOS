/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr_trigger_fault.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 23:41:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/20 19:33:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <system/isr.h>

#include <mm/mmu.h>

#if ISR_TRIGGERS == 1

/**
 * Trigger any kind of fault with
 *
 * __asm__ volatile("int $0x0"); // Causes a divide by zero exception
 *
 */

void isr_trigger_division_by_zero() {
	volatile int a = 1;
	volatile int b = 0;
	volatile int c = a / b; // Causes a division by zero
	(void)c;				// Prevents an unused variable warning
}

void isr_trigger_invalid_opcode() {
	__asm__ volatile("ud2"); // Causes an invalid opcode exception
}

void isr_trigger_page_fault() {
	if (!mmu_is_paging_enabled()) {
		__asm__ volatile("int $0xE");
		return;
	}
	__asm__ volatile(
		"movl $0x0, %eax\n\t"
		"movl (%eax), %eax" // Causes a page fault
	);
}

void isr_trigger_general_protection_fault() {
	__asm__ volatile(
		"mov $0x10, %ax\n\t" // Invalid segment for code
		"ltr %ax"			 // Loads an invalid TSS, causing a general protection fault
	);
}

void isr_trigger_stack_fault() {
	__asm__ volatile(
		"mov $0x10, %ax\n\t" // Invalid segment for stack
		"mov %ax, %ss"		 // Loads an invalid stack segment, causing a stack fault
	);
}

void isr_trigger_double_fault() {
	__asm__ volatile(
		"mov $0x10, %ax\n\t" // Invalid segment for code
		"ltr %ax"			 // Loads an invalid TSS, causing a double fault
	);
}

void isr_trigger_breakpoint() {
	__asm__ volatile("int $3"); // Causes a breakpoint exception
}

void isr_trigger_overflow() {
	__asm__ volatile("int $0x4"); // Causes an overflow exception
}

void isr_trigger_bound_range_exceed() {
	__asm__ volatile("bound %eax, 0x0"); // Causes a bound range exceed exception
}

void isr_trigger_invalid_tss() {
	__asm__ volatile("int $0xA"); // Causes an overflow exception
}
#endif
