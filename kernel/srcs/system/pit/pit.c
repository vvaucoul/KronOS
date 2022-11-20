/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:07:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:56:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/pit.h>
#include <asm/asm.h>

void speaker_phase(int hz)
{
    int divisor = __CHIPSET_FREQUENCY / hz;
    outb(__PIT_CMDREG, 0xb6);
    outb(__PIT_CHANNEL2, divisor & 0xFF);
    outb(__PIT_CHANNEL2, (divisor >> 8) & 0xFF);
}

void timer_phase(int hz)
{
    // This frequency is 1.1931816666 MHz
    int divisor = __CHIPSET_FREQUENCY / hz;
    outportb(__PIT_CMDREG, 0x36);
    outportb(__PIT_CHANNEL0, divisor & 0xFF);
    outportb(__PIT_CHANNEL0, divisor >> 8);
}

int timer_ticks = 0;
int timer_seconds = 0;

void timer_handler(struct regs *r)
{
    (void)r;
    timer_ticks++;

    if (timer_ticks % (__TIMER_HZ * 100) == 0)
    {
        timer_seconds++;
    }
}

void beep(unsigned int wait_time, unsigned int times)
{
    unsigned char tempA = inportb(0x61);
    unsigned char tempB = (inportb(0x61) & 0xFC);
    unsigned int count;

    for (count = 0; count == times; count++)
    {
        if (tempA != (tempA | 3))
        {
            outportb(0x61, tempA | 3);
        }
        timer_wait(wait_time); // Wait is one more PIT function
        outportb(0x61, tempB);
    }
}

void timer_install()
{
    timer_phase(__TIMER_HZ);
    speaker_phase(__TIMER_HZ);
    irq_install_handler(0, timer_handler);
}

void timer_wait(int ticks)
{
    int eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
        asm volatile("sti\n\thlt\n\tcld");
    asm volatile("sti");
}

void kpause(void)
{
    ASM_CLI();
    while (1)
        ;
}

void ksleep(int seconds)
{
    int eseconds;

    eseconds = timer_seconds + seconds;
    while (timer_seconds < eseconds)
        asm volatile("sti\n\thlt\n\tcld");
    asm volatile("sti");
}

void timer_display_ktimer(void)
{
    printk("%8%% Phase: "
            " %d\n",
            timer_ticks);
    printk("%8%% Seconds: %d\n", timer_seconds);
    printk("%8%% HZ: %d\n", (size_t)__TIMER_HZ);
}

#undef __PIT_CHANNEL0
#undef __PIT_CHANNEL1
#undef __PIT_CHANNEL2
#undef __PIT_CMDREG

#undef __CHIPSET_FREQUENCY
#undef __TIMER_HZ