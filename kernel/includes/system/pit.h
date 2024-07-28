/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:06:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/28 23:40:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIT_H
#define PIT_H

/**
 * This file contains the function prototypes and constants for the PIT module.
 * The PIT module provides a timer that generates periodic interrupts at a specified frequency.
 */

#include <limits.h>
#include <stdint.h>

#define PIT_COMMAND 0x43 // PIT Chip's Command Register Port

#define PIT_CHANNEL_0 0x40 // PIT Chip's Channel 0 Port
#define PIT_CHANNEL_1 0x41 // PIT Chip's Channel 1 Port
#define PIT_CHANNEL_2 0x42 // PIT Chip's Channel 2 Port

#define PIT_MASK 0xFF // Mask for PIT Chip

#define PIT_FREQUENCY 1193182 // The frequency of the PIT chip
#define TIMER_PHASE 1000 // Timer frequency in HZ
#define PIT_DIVISOR (PIT_FREQUENCY / TIMER_PHASE)

#define PIT_MAX_TICKS (__UINT64_MAX__) // Max ticks before overflow

extern void pit_setup();
extern void pit_wait(uint64_t ticks);
extern void busy_wait(uint64_t ticks);

#define ksleep(seconds) pit_wait((seconds) * TIMER_PHASE)
#define kmsleep(milliseconds) pit_wait((milliseconds) * TIMER_PHASE / 1000)
#define kusleep(microseconds) pit_wait((microseconds) * TIMER_PHASE / 1000000)

extern void kpause(void);
extern void pit_display_phase(void);

extern uint32_t read_pit_count(void);
extern uint64_t pit_get_ticks(void);
extern uint64_t pit_get_subticks(void);
extern uint64_t pit_get_continous_ticks(void);

#endif /* !PIT_H */