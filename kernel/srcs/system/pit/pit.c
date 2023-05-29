/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:07:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 16:15:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <asm/asm.h>
#include <multitasking/scheduler.h>
#include <system/pit.h>

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
    outportb(__PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint32_t timer_ticks = 0;
uint32_t timer_useconds = 0;
uint32_t timer_seconds = 0;

void timer_handler(struct regs *r)
{
    __UNUSED(r);
    timer_ticks++;

    if (timer_ticks % (__TIMER_HZ * 100) == 0)
    {
        timer_seconds++;
    }
    else if (timer_ticks % __TIMER_HZ == 0)
    {
        timer_useconds++;
    }

    // Check if we need to call the scheduler from pit
    if (timer_ticks % (__TIMER_HZ * 50) == 0)
    {
        if (scheduler_initialized == true) // 1000 = 1 second
            scheduler();
        // else
            // printk("Scheduler not initialized\n");
    }
    // scheduler(r->ebp, r->esp);

    /* Call the scheduler */
    // if (timer_ticks % __TIMER_HZ == 0) // 1000 = 1 second
    // scheduler(r->ebp, r->esp);
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
    uint32_t eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
        __asm__ volatile("sti\n\thlt\n\tcld");
    __asm__ volatile("sti");
}

void kpause(void)
{
    ASM_CLI();
    while (1)
    {
        __asm__ volatile("nop");
    }
}

void ksleep(int seconds)
{
    uint32_t eseconds;

    eseconds = timer_seconds + seconds;
    while (timer_seconds < eseconds)
        __asm__ volatile("sti\n\thlt\n\tcld");
    __asm__ volatile("sti");
}

void kusleep(int microseconds)
{
    uint32_t euseconds;

    euseconds = timer_useconds + microseconds;
    while (timer_useconds < euseconds)
        __asm__ volatile("sti\n\thlt\n\tcld");
    __asm__ volatile("sti");
}

void kmsleep(int milliseconds)
{
    uint32_t emilliseconds;

    emilliseconds = timer_useconds + (milliseconds * 1000);
    while (timer_useconds < emilliseconds)
        __asm__ volatile("sti\n\thlt\n\tcld");
    __asm__ volatile("sti");
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