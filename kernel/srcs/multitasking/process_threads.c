/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_threads.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 20:50:21 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/26 21:23:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>

void __process_threads(task_t *current_task) {
    if (current_task->threads != NULL) {
        thread_schedule(current_task->threads);
        // thread_t *tmp = current_task->threads;
        // while (tmp) {
        //     printk("[TASKING]: Starting thread\n");
        //     tmp = tmp->next;
        // }
    }
}