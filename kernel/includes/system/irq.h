/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:54:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/06 22:58:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRQ_H
#define IRQ_H

#include <kernel.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/io.h>
#include <system/panic.h>

/* Interrupts Request */

/*
+--------------------+------------+------------+------------+-------+
| Error              | Code       | Type       | Exception  | Zero? |
+--------------------+------------+------------+------------+-------+
| Divide-by-zero     | 0 (0x0)    | Fault      | #DE        | No    |
| Debug              | 1 (0x1)    | Fault/Trap | #DB        | No    |
| Non-maskable       | 2 (0x2)    | Interrupt  | -          | No    |
| Breakpoint         | 3 (0x3)    | Trap       | #BP        | No    |
| Overflow           | 4 (0x4)    | Trap       | #OF        | No    |
| Bound Range Exceed | 5 (0x5)    | Fault      | #BR        | No    |
| Invalid Opcode     | 6 (0x6)    | Fault      | #UD        | No    |
| Device Not         | 7 (0x7)    | Fault      | #NM        | No    |
| Double Fault       | 8 (0x8)    | Abort      | #DF        | Yes   |
| Coprocessor        | 9 (0x9)    | Fault      | -          | No    |
| Invalid TSS        | 10 (0xA)   | Fault      | #TS        | Yes   |
| Segment Not        | 11 (0xB)   | Fault      | #NP        | Yes   |
| Stack-Segment      | 12 (0xC)   | Fault      | #SS        | Yes   |
| General Protection | 13 (0xD)   | Fault      | #GP        | Yes   |
| Page Fault         | 14 (0xE)   | Fault      | #PF        | Yes   |
| Reserved           | 15 (0xF)   | -          | -          | No    |
| x87 Floating-Point | 16 (0x10)  | Fault      | #MF        | No    |
| Alignment Check    | 17 (0x11)  | Fault      | #AC        | Yes   |
| Machine Check      | 18 (0x12)  | Abort      | #MC        | No    |
| SIMD Floating-Point| 19 (0x13)  | Fault      | #XM/#XF    | No    |
| Virtualization     | 20 (0x14)  | Fault      | #VE        | No    |
| Control Protection | 21 (0x15)  | Fault      | #CP        | Yes   |
| Reserved           | 22-27 (0x16| -          | -          | No    |
| Hypervisor         | 28 (0x1C)  | Fault      | #HV        | No    |
| VMM Communication  | 29 (0x1D)  | Fault      | #VC        | Yes   |
| Security           | 30 (0x1E)  | Fault      | #SX        | Yes   |
| Reserved           | 31 (0x1F)  | -          | -          | No    |
| Triple Fault       | -          | -          | -          | No    |
| FPU Error Interrupt| IRQ 13     | Interrupt  | #FERR      | No    |
+--------------------+------------+------------+------------+-------+
*/

/* PICS */
#define MASTER_PIC 0x20
#define SLAVE_PIC 0xA0

#define MASTER_DATA (MASTER_PIC + 1)
#define SLAVE_DATA (SLAVE_PIC + 1)

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

#define ICW3_MASTER 0x04 /* IRQ2 -> connection to slave */
#define ICW3_SLAVE 0x02  /* IRQ2 <- connection from master */

#define MASTER_OFFSET 0x20 /* IRQ0-7 mapped to 0x20-0x27 */
#define SLAVE_OFFSET 0x28 /* IRQ8-15 mapped to 0x28-0x2F */

/* End of Interrupt */
#define IRQ_EOI 0x20

void irq_install();
void irq_handler(struct regs *r);
void irq_install_handler(int irq, void (*handler)(struct regs *r));

extern panic_t exceptions_errors[32];

#endif /* !IRQ_H */