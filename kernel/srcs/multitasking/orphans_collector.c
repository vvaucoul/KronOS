/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   orphans_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 11:05:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/24 00:41:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/scheduler.h>

extern task_t *ready_queue;

/**
 * @brief Collect orphans tasks
 * @param current_task
 *
 * @note : Collect orphans tasks from the ready_queue and attach them to the INIT task
 *         Remove Zombie tasks from the ready_queue
 */
void __orphans_collector(task_t *init_task) {
    if (init_task) {
        task_t *tmp = ready_queue; // Start from the beginning of the process list

        while (tmp) {
            if (tmp->state == TASK_ORPHAN) {
                printk(_YELLOW "Orphan task found, attaching "_GREEN
                               "[%d]"_END
                               " to INIT task"_END
                               "\n",
                       tmp->pid);
                tmp->state = TASK_RUNNING;
                tmp->ppid = INIT_PID; // Change parent to init
            }
            tmp = tmp->next;
        }
    }
}
