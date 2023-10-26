/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_waiting.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 17:16:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/26 17:18:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/scheduler.h>

/**
 * @brief Process waiting queue
 * @note : Check if a task in the waiting queue is ready to be added to the ready queue
 *        If so, add it to the ready queue
 *       Else, continue
 * @note : This function is called by the scheduler
 */
void __process_waiting(void) {

    task_t *tmp = waiting_queue;

    while (tmp) {
        if (get_task_count() < MAX_TASKS) {
            printk("Task [%d] is ready, add it to ready_queue\n", tmp->pid);
            __waiting_queue_remove_task(tmp);
            __ready_queue_add_task(tmp);
            tmp = tmp->next;
        } else {
            break;
        }
    }
}