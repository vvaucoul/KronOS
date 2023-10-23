/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_killer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 20:33:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/24 01:37:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

extern task_t *ready_queue;

int32_t __process_killer(void) {
    // return (0); //tmp
    task_t *tmp = ready_queue;

    while (tmp) {
        if (tmp->pid == 0 || tmp->pid == INIT_PID) {
            tmp = tmp->next;
            continue;
        }
        if (tmp->state == TASK_STOPPED) {
            task_t *next = tmp->next;
            printk(_YELLOW "Killing task "_GREEN
                           "[%d]"_END
                           "\n",
                   tmp->pid);
            int ret = free_task(tmp);

            // if (ret)
            //     return (ret);
            
            tmp = next;
            return (ret);
        } else {
            tmp = tmp->next;
        }
    }
    return (0);
}