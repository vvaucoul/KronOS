/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 12:21:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 21:00:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <syscall/wait.h>

static pid_t __wait(pid_t pid, int *status, int options) {

    __UNUSED(options);

    // If pid == -1, wait for any child process
    if (pid == -1) {
        task_t *child;

        do {
            for (uint32_t i = 0; i < MAX_TASKS; i++) {
                child = get_task(i);

                if (get_task_count() == 0) {
                    *status = -1; // No child
                    return (-1);  // No child
                } else if (!child) {
                    continue; // No child
                } else if (child->pid == INIT_PID) {
                    continue; // Cant' wait for Kernel INIT Process
                }

                if (child && (child->state == TASK_ZOMBIE || child->state == TASK_STOPPED)) {
                    *status = child->exit_code;
                    return (child->pid);
                }
            }
            busy_wait(TASK_FREQUENCY);
        } while (1);
    }
    // If pid >= 0, wait for the child process with the given pid
    else {
        if ((task_wait(pid)) < 0) {
            *status = -1;
            return (-1);
        } else {
            if (get_task(pid) == NULL)
                return (-1);
            *status = get_task(pid)->exit_code;
        }
    }
    return (pid);
}

pid_t wait(int *status) {
    pid_t pid = get_current_task()->pid;

    return __wait(pid, status, 0);
}

pid_t waitpid(pid_t pid, int *status, int options) {
    return __wait(pid, status, options);
}