/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_zombies.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 21:11:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/26 13:32:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

extern task_t *ready_queue;

int32_t __process_zombie(task_t *current_task) {
    task_t *tmp = ready_queue;
    __UNUSED(current_task);

    while (tmp) {
        if (tmp->pid == 0 || tmp->pid == INIT_PID) {
            tmp = tmp->next;
            continue;
        }
        if (tmp->state == TASK_ZOMBIE) {
            if (tmp->zombie_hungry >= ZOMBIE_HUNGRY_DIE) {

                task_t *next = tmp->next;
                printk(_YELLOW "Killing task "_GREEN
                               "[%d]"_END
                               "\n",
                       tmp->pid);

                /* Secure, Kill task only if it's not the current task */
                if (tmp->pid != current_task->pid) {
                    int ret = free_task(tmp);

                    if (ret)
                        return ret;
                }
                tmp = next;

            } else {
                tmp->zombie_hungry += ZOMBIE_HUNGRY;
                printk(_YELLOW "Task "_GREEN
                               "[%d]"_YELLOW
                               " is a zombie, waiting for it to die, Hungry: "_GREEN
                               "[%d]"_END
                               "\n",
                       tmp->pid, tmp->zombie_hungry);
                tmp = tmp->next;
            }
        } else {
            tmp = tmp->next;
        }
    }
    return (0);
}