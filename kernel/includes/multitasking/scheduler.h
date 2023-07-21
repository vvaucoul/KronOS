/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 11:26:53 by vvaucoul         ###   ########.fr       */
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

#ifdef __DEBUG__
#define __DEBUG_TASK_FREQUENCY 15
#define TASK_FREQUENCY __DEBUG_TASK_FREQUENCY
#else
#define TASK_FREQUENCY 1
#endif

extern bool scheduler_initialized;

extern void init_scheduler(void);

extern task_t *__process_selector(task_t *current_task);
extern void __orphans_collector(task_t *current_task);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     SIGNALS                                    ||
// ! ||--------------------------------------------------------------------------------||

void __signal_handler(task_t *current_task);

extern void task_add_signal(task_t *task, int signum, void (*handler)(int));
extern void task_remove_signal(task_t *task, int signum);

#endif /* !SCHEDULER_H */