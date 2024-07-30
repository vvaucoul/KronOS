/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fpu.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 16:53:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 11:50:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <system/fpu.h>

/**
 * Comments:
 * Enable Floating-Point Exceptions:
 *
 * The CR0 control register is modified to enable floating-point exceptions.
 * Enable SIMD Instructions:
 *
 * The CR4 control register is modified to enable the usage of SSE instructions, which can improve SIMD operations performance.
 * FPU Initialization:
 *
 * The fninit command initializes the FPU and loads the default control word.
 */

/**
 * @brief Enable the Floating Point Unit (FPU).
 *
 * This function enables the Floating Point Unit (FPU) on the system.
 * The FPU is responsible for performing floating-point arithmetic operations.
 *
 * @note This function assumes that the FPU is present on the system.
 *
 * @return void
 */
void enable_fpu(void) {
	// Enable floating-point exceptions
	unsigned int cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x10; // Enable floating-point exception handling
	__asm__ volatile("mov %0, %%cr0" : : "r"(cr0));

	// Enable FPU and SIMD instructions usage
	unsigned int cr4;
	__asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200; // Enable SSE (Streaming SIMD Extensions) usage
	__asm__ volatile("mov %0, %%cr4" : : "r"(cr4));

	// Initialize FPU, load default control word
	__asm__ volatile("fninit");
}

/**
 * @brief Check if the FPU (Floating Point Unit) is present.
 *
 * @return 1 if the FPU is present, 0 otherwise.
 */
int fpu_is_present(void) {
	unsigned int cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	return (cr0 & 0x10);
}