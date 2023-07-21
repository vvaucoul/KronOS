/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_selector.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 10:20:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 10:43:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

extern task_t *ready_queue;

task_t *__process_selector(task_t *current_task) {
    if (current_task) {
        current_task = current_task->next;
        if (!current_task)
            current_task = ready_queue;
        // if (current_task->state != TASK_RUNNING)
            // return __process_selector(current_task->next);
    } else
        current_task = ready_queue;
    return current_task;
}