/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_sleeping.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 11:43:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/17 01:07:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <system/pit.h>

extern task_t *ready_queue;

static bool __process_is_in_queue_list(task_t *current_task) {
	task_t *tmp = ready_queue;

	do {
		if (tmp == current_task)
			return (true);
		tmp = tmp->next;
		if (!tmp) {
			tmp = ready_queue;
		}
	} while (tmp != ready_queue);
	return (false);
}

void __process_sleeping(task_t *current_task) {
	// If there are no tasks, return
	if (!current_task)
		return;

	if (__process_is_in_queue_list(current_task) == false)
		return;

	// Start from the beginning of the queue
	task_t *tmp = current_task;

	// Loop through all tasks
	do {
		// If the task is sleeping and its wake-up tick has passed, wake it up
		if (tmp->state == TASK_SLEEPING && tmp->wake_up_tick <= pit_get_subticks()) {
			// printk("Process %d woke up\n", tmp->pid);
			tmp->state = TASK_RUNNING;
			tmp->wake_up_tick = 0;
		} else if (tmp->state == TASK_SLEEPING && tmp->wake_up_tick > pit_get_subticks()) {
			// printk("Process %d is sleeping for %d ticks / %d\n", tmp->pid, tmp->wake_up_tick, pit_get_subticks());
		}

		// Move to the next task
		tmp = tmp->next;

		if (!tmp)
			tmp = ready_queue;

		// If we've reached the end of the queue, break the loop
	} while (tmp != current_task);
}