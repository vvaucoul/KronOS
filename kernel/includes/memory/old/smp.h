/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smp.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 15:43:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 15:52:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SMP_H
#define SMP_H

#include <kernel.h>

/*
** SMP: Supervisor Memory Protection
**
** SMAP: Supervisor Memory Access Protection
** SMEP: Supervisor Memory Execution Protection
*/

#define CPUID_SMEP 7
#define CPUID_SMAP 20
#define CPU_CR4_SMEP_BIT 20
#define CPU_CR4_SMAP_BIT 21

// Set AC bit in RFLAGS register.
#define SMP_SET_AC() asm volatile("stac" :: \
                                      : "cc")
// Clear AC bit in RFLAGS register.
#define SMP_CLEAR_AC() asm volatile("clac" :: \
                                        : "cc")

extern void smp_init(void);

#endif /* !SMP_H */