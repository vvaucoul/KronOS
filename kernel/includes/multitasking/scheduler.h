/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 22:40:38 by vvaucoul         ###   ########.fr       */
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
#define TASK_FREQUENCY 15

extern bool scheduler_initialized;

extern void init_scheduler(void);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     SIGNALS                                    ||
// ! ||--------------------------------------------------------------------------------||

void __signal_handler(task_t *current_task);

extern void task_add_signal(task_t *task, int signum, void (*handler)(int));
extern void task_remove_signal(task_t *task, int signum);

#endif /* !SCHEDULER_H */