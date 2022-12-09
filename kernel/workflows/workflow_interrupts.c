/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_interrupts.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 21:57:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 00:02:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>
#include <kernel.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/irq.h>
#include <system/panic.h>

#include <system/pit.h>

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void interrupts_test(void)
{
    /* FAULT - INTERRUPTS - TRAPS */

    /* Division by zero */
    __asm__ volatile("int $0x0");

    /* Debug */
    __asm__ volatile("int $0x1");

    /* Non Maskable Interrupt */
    __asm__ volatile("int $0x2");

    /* Breakpoint */
    __asm__ volatile("int $0x3");

    /* Overflow */
    __asm__ volatile("int $0x4");

    /* Bound Range Exceeded */
    __asm__ volatile("int $0x5");

    /* Invalid Opcode */
    __asm__ volatile("int $0x6");

    /* Device Not Available */
    __asm__ volatile("int $0x7");

    /* Coprocessor Segment Overrun */
    __asm__ volatile("int $0x9");

    /* Invalid TSS */
    __asm__ volatile("int $0x0a");

    /* Segment Not Present */
    __asm__ volatile("int $0x0b");

    /* PANICS */

    /* Double Fault */
    __asm__ volatile("int $0x8");

    /* Page Fault */
    __asm__ volatile("int $0x0e");
}