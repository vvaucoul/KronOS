/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 18:49:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISR_H
#define ISR_H

#include <kernel.h>
#include <system/idt.h>

#define ISR_MAX_COUNT 32

typedef struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
} t_regs;

typedef void (*ISR)(t_regs *);

#define NB_INTERRUPT_HANDLERS 256

extern ISR g_interrupt_handlers[NB_INTERRUPT_HANDLERS];

void isrs_install();

#endif // !ISR_H