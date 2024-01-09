/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 12:23:27 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 12:24:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WAIT_H
#define WAIT_H

#include <kernel.h>
#include <multitasking/process.h>

/*
 * wait() - Suspend execution until a child process terminates.
 *
 * Returns:
 *   - On success: The process ID (PID) of the terminated child process.
 *   - On failure: -1, indicating an error occurred.
 *
 * Description:
 *   The wait() system call suspends the execution of the calling process
 *   until one of its child processes terminates. It allows a parent process
 *   to wait for the completion of its child processes and retrieve information
 *   about their exit status.
 *
 *   The waitpid() function is a variation of wait() that provides more control
 *   over which child process to wait for and how to retrieve information about
 *   its termination.
 */

extern pid_t wait(int *status);
extern pid_t waitpid(pid_t pid, int *status, int options);

#endif /* !WAIT_H */