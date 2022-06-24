/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:07:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/23 00:08:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/system/pit.h"

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;     /* Calculate our divisor */
    outportb(0x43, 0x36);           /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF); /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);   /* Set high byte of divisor */
}

/* This will keep track of how many ticks that the system
 *  has been running for */
int timer_ticks = 0;

/* Handles the timer. In this case, it's very simple: We
 *  increment the 'timer_ticks' variable every time the
 *  timer fires. By default, the timer fires 18.222 times
 *  per second. Why 18.222Hz? Some engineer at IBM must've
 *  been smoking something funky */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    (void)r;
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
     *  display a message on the screen */
    if (timer_ticks % 18 == 0)
    {
        // kputs("One second has passed\n");
    }
    char buffer[__KITOA_BUFFER_LENGTH__];

    terminal_writestring_location("Timer: ", VGA_WIDTH - kstrlen("Timer: ") - 5, 0);
    terminal_writestring_location("Phase: ", VGA_WIDTH - kstrlen("Phase: ") - 5, 1);

    kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    kitoa(timer_ticks / 18, buffer);
    terminal_writestring_location(buffer, VGA_WIDTH - kstrlen(buffer) - 1, 0);
    
    kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    kitoa(timer_ticks, buffer);
    terminal_writestring_location(buffer, VGA_WIDTH - kstrlen(buffer) - 1, 1);
}

void timer_install()
{
    irq_install_handler(0, timer_handler);
}

/* This will continuously loop until the given time has
 *  been reached */
void timer_wait(int ticks)
{
    int eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
    {
    }
}

void ksleep(int seconds)
{
    timer_wait(seconds);
}