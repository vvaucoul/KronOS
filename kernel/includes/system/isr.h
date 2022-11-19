/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 19:53:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISR_H
#define ISR_H

#include <kernel.h>
#include <system/idt.h>

#define ISR_MAX_COUNT 32

typedef struct regs
{
    /* Segment registers */
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    /* 32-bit registers */
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} t_regs;

typedef struct s_regs_16
{
    /* 16-bit registers */
    uint16_t di;
    uint16_t si;
    uint16_t bp;
    uint16_t sp;
    uint16_t bx;
    uint16_t dx;
    uint16_t cx;
    uint16_t ax;

    /* segments */
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;
    uint16_t eflags;
} regs16_t;

typedef void (*ISR)(t_regs *);

#define NB_INTERRUPT_HANDLERS 256

extern ISR g_interrupt_handlers[NB_INTERRUPT_HANDLERS];

void isrs_install();
void isr_register_interrupt_handler(int num, ISR handler);

#endif // !ISR_H