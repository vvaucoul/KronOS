/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 12:21:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 12:47:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <syscall/wait.h>

static pid_t __wait(pid_t pid, int *status, int options) {

    __UNUSED(options);

    //Todo: Implement options
    // PID = -1 => Wait for any child process
    

    if ((task_wait(pid)) < 0) {
        return (-1);
    } else {
        *status = get_current_task()->exit_code;
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