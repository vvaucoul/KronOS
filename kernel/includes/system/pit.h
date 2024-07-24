/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:06:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 13:34:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIT_H
#define PIT_H

#include "../kernel.h"
#include "idt.h"
#include "io.h"
#include "irq.h"
#include "isr.h"

#define PIT_CMDREG 0x43 // PIT Chip's Command Register Port

#define PIT_CHANNEL_0 0x00               // 00......
#define PIT_CHANNEL_1 0x40               // 01......
#define PIT_CHANNEL_2 0x80               // 10......
#define PIT_CHANNEL_READBACK 0xC0        // 11......
#define PIT_ACCESS_LATCHCOUNT 0x00       // ..00....
#define PIT_ACCESS_LOBYTE 0x10           // ..01....
#define PIT_ACCESS_HIBYTE 0x20           // ..10....
#define PIT_ACCESS_LOHIBYTE 0x30         // ..11....
#define PIT_OPMODE_0_IOTC 0x00           // ....000.
#define PIT_OPMODE_1_ONESHOT 0x02        // ....001.
#define PIT_OPMODE_2_RATE_GEN 0x04       // ....010.
#define PIT_OPMODE_3_SQUARE_WAV 0x06     // ....011.
#define PIT_OPMODE_4_SOFTWARESTROBE 0x08 // ....100.
#define PIT_OPMODE_4_HARDWARESTROBE 0x0A // ....101.
#define PIT_OPMODE_4_RATE_GEN 0x0C       // ....110.
#define PIT_OPMODE_4_SQUARE_WAV 0x0E     // ....111.
#define PIT_BINARY 0x00                  // .......0
#define PIT_BCD 0x01                     // .......1

#define PIT_MASK 0xFF
#define PIT_SET 0x36

#define __CHIPSET_FREQUENCY 1193180 // The frequency of the PIT chip
// #define TIMER_PHASE (int)(18.2065)
#define TIMER_PHASE 18 // Timer frequency in HZ
#define TIMER_FREQUENCY (uint32_t)(__CHIPSET_FREQUENCY / TIMER_PHASE)
#define TIMER_MAX_TICKS (uint32_t)(0xFFFFFFFF / TIMER_FREQUENCY) // Max ticks before overflow

extern void timer_install();
extern void timer_handler(struct regs *r);
extern void timer_wait(uint32_t ticks);
extern void busy_wait(uint32_t ticks);

#define ksleep(seconds) timer_wait(seconds *TIMER_PHASE)
#define kusleep(microseconds) timer_wait((microseconds * TIMER_PHASE) / 1000000)
#define kmsleep(milliseconds) timer_wait((milliseconds * TIMER_PHASE) / 1000)

extern void kpause(void);

extern uint32_t timer_ticks;
extern uint32_t timer_subtick;

extern void timer_display_ktimer(void);

extern uint32_t pit_get_ticks(void);

#endif /* !PIT_H */