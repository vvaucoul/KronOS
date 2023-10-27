/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/26 20:51:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <memory/memory.h>
#include <multitasking/process.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                      MULTILEVEL FEEDBACK QUEUE SCHEDULING                      ||
// ! ||--------------------------------------------------------------------------------||

// #define __DEBUG__ 1

#ifdef __DEBUG__
#define MAX_TASKS 4
#else
#define MAX_TASKS 32 // 4 tasks max
#endif
#define PID_MAX __INT32_MAX__ // Max pid

#ifdef __DEBUG__
#define __DEBUG_TASK_FREQUENCY 1
#define TASK_FREQUENCY __DEBUG_TASK_FREQUENCY
#else
#define TASK_FREQUENCY 1
#endif

extern bool scheduler_initialized;

extern void init_scheduler(void);

extern task_t *__process_selector(task_t *current_task);
extern void __orphans_collector(task_t *current_task);
extern void __process_sleeping(task_t *current_task);
extern int32_t __process_killer(void);
extern int32_t __process_zombie(task_t *current_task);
extern void __process_waiting(void);
extern void __process_threads(task_t *current_task);

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

extern void __task_overflow_handler(void);

#endif /* !SCHEDULER_H */