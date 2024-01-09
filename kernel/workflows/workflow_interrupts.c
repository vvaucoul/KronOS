/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_interrupts.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 21:57:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:03 by vvaucoul         ###   ########.fr       */
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
