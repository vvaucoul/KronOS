/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 11:39:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <memory/memory.h>
#include <multitasking/process.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                      MULTILEVEL FEEDBACK QUEUE SCHEDULING                      ||
// ! ||--------------------------------------------------------------------------------||

#define MAX_TASKS 4 // 4 tasks max
#define PID_MAX __INT32_MAX__ // Max pid

//#define __DEBUG__ 0

#ifdef __DEBUG__
#define __DEBUG_TASK_FREQUENCY 4
#define TASK_FREQUENCY __DEBUG_TASK_FREQUENCY
#else
#define TASK_FREQUENCY 1
#endif

extern bool scheduler_initialized;

extern void init_scheduler(void);

extern task_t *__process_selector(task_t *current_task);
extern void __orphans_collector(task_t *current_task);
extern void __process_sleeping(task_t *current_task);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     SIGNALS                                    ||
// ! ||--------------------------------------------------------------------------------||

void __signal_handler(task_t *current_task);

extern void task_add_signal(task_t *task, int signum, void (*handler)(int));
extern void task_remove_signal(task_t *task, int signum);
extern void task_print_signals(task_t *task);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     ROUNDED                                    ||
// ! ||--------------------------------------------------------------------------------||

extern void wait_for_scheduler_rounded(void);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    OVERFLOW                                    ||
// ! ||--------------------------------------------------------------------------------||

extern void task_overflow_handler(void);

#endif /* !SCHEDULER_H */