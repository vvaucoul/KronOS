/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_monitor.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 13:02:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 14:31:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <kronos/stack_monitor.h>
#include <multiboot/multiboot_mmap.h>

/**
 * @brief Get the stack usage.
 *
 * This function returns the stack usage in bytes.
 *
 * @return The stack usage in bytes.
 */
uint32_t sm_get_stack_usage(void) {
	uint32_t s_base, s_top, s_size;
	uint32_t count = 0;

	s_base = sm_get_stack_base();
	s_top = sm_get_stack_top();
	s_size = sm_get_stack_size();

	for (uint32_t i = s_base; i < s_top && i < s_base + s_size; i += sizeof(uint32_t)) {
		uint32_t *ptr = (uint32_t *)(uintptr_t)i;

		if (*ptr == sm_get_stack_marker()) {
			break;
		} else {
			count++;
		}
	}

	return (count);
}

/**
 * @brief Calculates the percentage of stack usage.
 *
 * This function calculates the percentage of stack usage based on the current stack pointer
 * and the stack size. The result represents the amount of stack space that has been used.
 *
 * @return The percentage of stack usage.
 */
uint32_t sm_get_stack_usage_percentage(void) {
	uint32_t s_size, s_usage;

	s_size = sm_get_stack_size();
	s_usage = sm_get_stack_usage();

	return ((s_usage * 100) / s_size);
}

/**
 * @brief Retrieves the size of the stack.
 *
 * This function returns the size of the stack in bytes.
 *
 * @return The size of the stack in bytes.
 */
uint32_t sm_get_stack_size(void) {
	return (KERNEL_STACK_SIZE);
}

/**
 * @brief Retrieves the base address of the stack.
 *
 * This function returns the base address of the stack.
 *
 * @return The base address of the stack.
 */
uint32_t sm_get_stack_base(void) {
	uint32_t esp;

	__asm__ volatile("mov %%esp, %0" : "=r"(esp));
	return (esp);
}

/**
 * @brief Retrieves the top address of the stack.
 *
 * This function returns the top address of the stack.
 *
 * @return The top address of the stack.
 */
uint32_t sm_get_stack_top(void) {
	uint32_t esp;

	__asm__ volatile("mov %%esp, %0" : "=r"(esp));
	return (esp + KERNEL_STACK_SIZE);
}

/**
 * @brief Retrieves the stack marker.
 *
 * This function returns the stack marker value.
 *
 * @return The stack marker value.
 */
uint32_t sm_get_stack_marker(void) {
	return (KERNEL_STACK_MARKER);
}

/**
 * @brief Prints stack information.
 *
 * This function is responsible for printing stack information.
 * It can be used to monitor the stack usage in a program.
 */
void sm_print_stack_info(void) {
	uint32_t s_base, s_top, s_size, s_usage, s_usage_percentage;

	s_base = sm_get_stack_base();
	s_top = sm_get_stack_top();
	s_size = sm_get_stack_size();
	s_usage = sm_get_stack_usage();
	s_usage_percentage = sm_get_stack_usage_percentage();

	printk(_YELLOW "Stack Info:\n" _END);
	printk("\t- Base: 0x%x\n", s_base);
	printk("\t- Top: 0x%x\n", s_top);
	printk("\t- Size: 0x%x\n", s_size);
	printk("\t- Usage: 0x%x\n", s_usage);
	printk("\t- Usage Percentage: %u%%\n", s_usage_percentage);
}