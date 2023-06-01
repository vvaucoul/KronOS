/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:07:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 16:35:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <asm/asm.h>
#include <multitasking/scheduler.h>
#include <system/pit.h>

void speaker_phase(int hz) {
    int divisor = __CHIPSET_FREQUENCY / hz;
    outb(__PIT_CMDREG, 0xb6);
    outb(__PIT_CHANNEL2, divisor & PIT_MASK);
    outb(__PIT_CHANNEL2, (divisor >> 8) & PIT_MASK);
}

void timer_phase(int hz) {
    // This frequency is 1.1931816666 MHz
    int divisor = __CHIPSET_FREQUENCY / hz;
    outportb(__PIT_CMDREG, PIT_SET);
    outportb(__PIT_CHANNEL0, divisor & PIT_MASK);
    outportb(__PIT_CHANNEL0, (divisor >> 8) & PIT_MASK);
}

uint32_t timer_ticks = 0;
uint32_t timer_subtick = 0;

void timer_handler(struct regs *r) {

    __UNUSED(r);

    timer_subtick++;

    if (timer_subtick == __TIMER_HZ) {
        timer_ticks++;
        timer_subtick = 0;
    }

    if (timer_subtick % TASK_FREQUENCY == 0 && scheduler_initialized) {
        switch_task();
    }
}

void beep(unsigned int wait_time, unsigned int times) {
    unsigned char tempA = inportb(0x61);
    unsigned char tempB = (inportb(0x61) & 0xFC);
    unsigned int count;

    for (count = 0; count == times; count++) {
        if (tempA != (tempA | 3)) {
            outportb(0x61, tempA | 3);
        }
        timer_wait(wait_time); // Wait is one more PIT function
        outportb(0x61, tempB);
    }
}

void timer_install() {
    timer_phase(__TIMER_HZ);
    speaker_phase(__TIMER_HZ);
    irq_install_handler(IRQ_PIT, timer_handler);
}

void timer_wait(uint32_t ticks) {
    uint32_t start_tick = timer_subtick;

    while (timer_subtick - start_tick < ticks) {
        __asm__ volatile("sti\n\thlt\n\tcld");
    }
}

void kpause(void) {
    ASM_CLI();
    while (1) {
        __asm__ volatile("nop");
    }
}

void timer_display_ktimer(void) {
    printk("%8%% Phase: "
           " %d\n",
           timer_subtick);
    printk("%8%% Seconds: %d\n", timer_ticks);
    printk("%8%% HZ: %d\n", (size_t)__TIMER_HZ);
}

#undef __PIT_CHANNEL0
#undef __PIT_CHANNEL1
#undef __PIT_CHANNEL2
#undef __PIT_CMDREG

#undef __CHIPSET_FREQUENCY
#undef __TIMER_HZ