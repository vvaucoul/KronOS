/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pit.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 20:07:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 18:16:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <asm/asm.h>
#include <multitasking/scheduler.h>
#include <system/pit.h>

void speaker_phase(int hz) {
    int divisor = __CHIPSET_FREQUENCY / hz;
    outb(PIT_CMDREG, 0xb6);
    outb(PIT_CHANNEL_2, divisor & PIT_MASK);
    outb(PIT_CHANNEL_2, (divisor >> 8) & PIT_MASK);
}

static void __timer_phase(void) {
    // This frequency is 1.1931816666 MHz
    int divisor = __CHIPSET_FREQUENCY / TIMER_PHASE;
    outportb(PIT_CMDREG, PIT_SET);
    outportb(PIT_CHANNEL_0, (uint8_t)(divisor & PIT_MASK));
    outportb(PIT_CHANNEL_0, (uint8_t)((divisor >> 8) & PIT_MASK));
}

uint32_t timer_ticks = 0;
uint32_t timer_subtick = 0;

void timer_handler(struct regs *r) {

    __UNUSED(r);

    timer_subtick++;

    if (timer_subtick == TIMER_PHASE) {
        timer_ticks++;
    }

    if (timer_subtick % TIMER_MAX_TICKS == 0) {
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
    irq_install_handler(IRQ_PIT, timer_handler);
    __timer_phase();
    // speaker_phase(TIMER_PHASE);
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
    printk("%8%% HZ: %d\n", (size_t)TIMER_PHASE);
}