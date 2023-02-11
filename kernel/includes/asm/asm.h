/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   asm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 15:06:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 20:23:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASM_H
#define ASM_H

/*******************************************************************************
 *                                GET ASM FLAGS                                *
 ******************************************************************************/

#define GET_ESP(x) __asm__ volatile("mov %%esp, %0" \
								: "=r"(x)::)
#define GET_EBP(x) __asm__ volatile("mov %%ebp, %0" \
								: "=r"(x)::)
#define GET_EIP(x) __asm__ volatile("mov %%eip, %0" \
								: "=r"(x)::)
#define GET_ECX(x) __asm__ volatile("mov %%ecx, %0" \
								: "=r"(x)::)
#define GET_EDX(x) __asm__ volatile("mov %%edx, %0" \
								: "=r"(x)::)
#define GET_EBX(x) __asm__ volatile("mov %%ebx, %0" \
								: "=r"(x)::)
#define GET_EAX(x) __asm__ volatile("mov %%eax, %0" \
								: "=r"(x)::)
#define GET_ESI(x) __asm__ volatile("mov %%esi, %0" \
								: "=r"(x)::)
#define GET_EDI(x) __asm__ volatile("mov %%edi, %0" \
								: "=r"(x)::)

/*******************************************************************************
 *                                   EFLAGS                                    *
 ******************************************************************************/

#define GET_EFLAGS(x) __asm__ volatile("pushf\n\t" \
								   "pop %0"    \
								   : "=r"(x)::)
#define SET_EFLAGS(x) __asm__ volatile("push %0\n\t" \
								   "popf" ::     \
									   : "r"(x))

/*******************************************************************************
 *                                SET ASM FLAGS                                *
 ******************************************************************************/

#define SET_EIP(x) __asm__ volatile("mov %0, %%eip" :: \
									: "r"(x))
#define SET_ESP(x) __asm__ volatile("mov %0, %%esp" :: \
									: "r"(x))
#define SET_EBP(x) __asm__ volatile("mov %0, %%ebp" :: \
									: "r"(x))
#define SET_ECX(x) __asm__ volatile("mov %0, %%ecx" :: \
									: "r"(x))
#define SET_EDX(x) __asm__ volatile("mov %0, %%edx" :: \
									: "r"(x))
#define SET_EBX(x) __asm__ volatile("mov %0, %%ebx" :: \
									: "r"(x))
#define SET_EAX(x) __asm__ volatile("mov %0, %%eax" :: \
									: "r"(x))
#define SET_ESI(x) __asm__ volatile("mov %0, %%esi" :: \
									: "r"(x))
#define SET_EDI(x) __asm__ volatile("mov %0, %%edi" :: \
									: "r"(x))

/*******************************************************************************
 *                              SET ASM REGISTER                               *
 ******************************************************************************/

#define SET_CR4(x) __asm__ volatile("mov %0, %%cr4" :: \
									: "r"(x))

/*******************************************************************************
 *                                    CPUID                                    *
 ******************************************************************************/

#define CPUID(eax, ebx, ecx, edx) __asm__ volatile("cpuid"                                          \
											   : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) \
											   : "a"(*eax), "c"(*ecx))

/*******************************************************************************
 *                               INTERRUPT FLAG                                *
 ******************************************************************************/

#define ASM_STI() __asm__ volatile("sti") // Set Interrupt Flag
#define ASM_CLI() __asm__ volatile("cli") // Clear Interrupt Flag

#endif /* !ASM_H */