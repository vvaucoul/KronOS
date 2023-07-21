/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_selector.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 10:20:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 13:04:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

extern task_t *ready_queue;

task_t *__process_selector(task_t *current_task) {
    task_t *highest_priority_task = NULL;

    // If the current task is null, start from the beginning of the ready queue
    if (!current_task)
        current_task = ready_queue;

    task_t *tmp = current_task;

    do {
        // If the task is running and has a higher priority than the currently selected task,
        // select it instead
        
        if (tmp->state == TASK_RUNNING && (!highest_priority_task || tmp->priority > highest_priority_task->priority)) {
            highest_priority_task = tmp;
        }
        

        tmp = tmp->next;

        // If we've reached the end of the queue, start again at the beginning
        if (!tmp)
            tmp = ready_queue;

    // Keep looking until we've checked all tasks in the queue
    } while (tmp != current_task);

    // If we didn't find any running tasks, return the current task
    if (!highest_priority_task)
        return current_task;
        
    // printk("Process %d priority: %d, State %d\n", tmp->pid, tmp->priority, tmp->state);
        // kpause();

    return highest_priority_task;
}