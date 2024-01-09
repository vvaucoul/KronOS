/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 11:29:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 12:24:38 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FORK_H
#define FORK_H

#include <kernel.h>
#include <multitasking/process.h>

/*
 * fork() - Create a new process by duplicating the calling process.
 *
 * Returns:
 *   - On success (in the parent process): the child's process ID (PID).
 *   - On success (in the child process): 0.
 *   - On failure: -1, indicating an error occurred.
 *
 * Description:
 *   The fork() system call creates a new process by duplicating the
 *   calling process (called the parent process). The new process is
 *   an exact copy of the parent process, including its code, data,
 *   file descriptors, etc. However, the parent and child processes
 *   run in separate memory spaces.
 *
 *   The fork() function is widely used to implement process creation
 *   in UNIX-like operating systems. It allows for the creation of
 *   complex program structures such as process trees or parallel processes.
 */

extern pid_t fork(void);

#endif /* !FORK_H */