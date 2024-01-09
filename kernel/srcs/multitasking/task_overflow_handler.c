/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   task_overflow_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 11:31:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <system/panic.h>
#include <kernel.h>

uint32_t stack_canary = 0xDEADBEEF;

void __task_overflow_handler(void) {
    uint32_t esp;

    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));

    if (stack_canary != 0xDEADBEEF || esp <= KERNEL_STACK_SIZE) {
        __PANIC("Stack overflow detected");
        kpause();
    }
}