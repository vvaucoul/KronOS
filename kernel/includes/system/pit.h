/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:06:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/24 13:33:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIT_H
# define PIT_H

#include "../kernel.h"
#include "idt.h"
#include "isr.h"
#include "io.h"
#include "irq.h"

#define __TIMER_HZ__ 18.222

extern void timer_install();
extern void timer_handler(struct regs *r);
extern void timer_wait(int ticks);
extern void ksleep(int seconds);

extern int timer_ticks;
extern int timer_seconds;

#endif // PIT_H