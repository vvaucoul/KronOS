/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:06:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 12:51:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIT_H
#define PIT_H

#include "../kernel.h"
#include "idt.h"
#include "io.h"
#include "irq.h"
#include "isr.h"

#define __PIT_CHANNEL0 0x40 // PIT Channel 0's Data Register Port
#define __PIT_CHANNEL1 0x41 // PIT Channels 1's Data Register Port
#define __PIT_CHANNEL2 0x42 // PIT Channels 2's Data Register Port
#define __PIT_CMDREG 0x43   // PIT Chip's Command Register Port

#define __CHIPSET_FREQUENCY 1193180 // The frequency of the PIT chip
#define __TIMER_HZ (int)(18.2065)

extern void timer_install();
extern void timer_handler(struct regs *r);
extern void timer_wait(int ticks);
extern void ksleep(int seconds);
extern void kusleep(int microseconds);
extern void kmsleep(int milliseconds);
extern void kpause(void);

extern uint32_t timer_ticks;
extern uint32_t timer_seconds;
extern uint32_t timer_useconds;

extern void timer_display_ktimer(void);

#endif /* !PIT_H */