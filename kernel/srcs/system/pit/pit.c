/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:07:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 11:21:57 by vvaucoul         ###   ########.fr       */
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

int timer_ticks = 0;
int timer_seconds = 0;

void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    (void)r;
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
     *  display a message on the screen */
    // if (timer_ticks % (int)((double)100 * (__TIMER_HZ__ / 2)) == 0)
    // if (timer_ticks % 1000 == 0)
    if (timer_ticks % 18 == 0)
    {
        timer_seconds++;
        // kputs("One second has passed");
    }
    char buffer[__KITOA_BUFFER_LENGTH__];

    terminal_writestring_location("Phase: ", VGA_WIDTH - kstrlen("Phase: ") - 7, 1);
    terminal_writestring_location("Sec: ", VGA_WIDTH - kstrlen("Sec: ") - 7, 2);

    kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    kitoa(timer_ticks, buffer);
    terminal_writestring_location(buffer, VGA_WIDTH - kstrlen(buffer) - 1, 1);

    kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    kitoa(timer_seconds, buffer);
    terminal_writestring_location(buffer, VGA_WIDTH - kstrlen(buffer) - 1, 2);
}

void timer_install()
{
    // timer_phase(__TIMER_HZ__);
    irq_install_handler(0, timer_handler);
}

void timer_wait(int ticks)
{
    int eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
    {
        kprintf("");
    }
}

void ksleep(int seconds)
{
    int eseconds;

    eseconds = timer_seconds + seconds;
    while (timer_seconds < eseconds)
    {
        // char buffer[__KITOA_BUFFER_LENGTH__];
        // kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
        // kitoa(eseconds - timer_seconds, buffer);
        // terminal_writestring_location(buffer, VGA_WIDTH - kstrlen(buffer) - 1, 4);
        kprintf("");
    }
}