/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:54:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 15:51:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRQ_H
#define IRQ_H

#include <kernel.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/io.h>
#include <system/panic.h>

/*******************************************************************************
 *                           INTERRUPT REQUESTS LIST                           *
 ******************************************************************************/

/*
+-------+---------------------------------------------------------+
| IRQ   | Description                                             |
+-------+---------------------------------------------------------+
| 0     | Programmable Interrupt Timer Interrupt                  |
| 1     | Keyboard Interrupt                                      |
| 2     | Cascade (used internally by the two PICs. never raised) |
| 3     | COM2 (if enabled)                                       |
| 4     | COM1 (if enabled)                                       |
| 5     | LPT2 (if enabled)                                       |
| 6     | Floppy Disk                                             |
| 7     | LPT1                                                    |
| 8     | CMOS real-time clock (if enabled)                       |
| 9     | Free for peripherals / legacy SCSI / NIC                |
| 10    | Free for peripherals / SCSI / NIC                       |
| 11    | Free for peripherals / SCSI / NIC                       |
| 12    | PS2 Mouse                                               |
| 13    | FPU / Coprocessor / Inter-processor                     |
| 14    | Primary ATA Hard Disk                                   |
| 15    | Secondary ATA Hard Disk                                 |
+-------+---------------------------------------------------------+
*/

/*******************************************************************************
 *                                    PICS                                     *
 ******************************************************************************/

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
#define SLAVE_OFFSET 0x28  /* IRQ8-15 mapped to 0x28-0x2F */

#define IRQ_EOI 0x20 /* End of Interrupt */

#define IRQ_PIT 0

/*******************************************************************************
 *                                IRQ FUNCTIONS                                *
 ******************************************************************************/

extern void irq_install();
extern void irq_handler(struct regs *r);
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void pic8259_send_eoi(uint8_t irq);

/*******************************************************************************
 *                                 EXCEPTIONS                                  *
 ******************************************************************************/

extern panic_t exceptions_errors[ISR_MAX_COUNT];

#endif /* !IRQ_H */