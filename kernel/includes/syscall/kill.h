/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 11:29:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 11:36:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KILL_H
#define KILL_H

#include <multitasking/process.h>
#include <shell/ksh_args.h>

/*
 * kill() - Send a signal to a process or a group of processes.
 *
 * Returns:
 *   - On success: 0.
 *   - On failure: -1, indicating an error occurred.
 *
 * Description:
 *   The kill() system call sends a signal to a process or a group of
 *   processes specified by the process ID (PID) or the process group ID.
 *   The signal can be used to request the termination of a process, handle
 *   exceptional conditions, or communicate with other processes.
 *
 *   The kill() function is a fundamental mechanism for process communication
 *   and control in UNIX-like operating systems. It allows processes to
 *   interact and coordinate their behavior through the exchange of signals.
 */

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 KILL FUNCTIONS                                 ||
// ! ||--------------------------------------------------------------------------------||

extern int builtin_kill(const ksh_args_t *args);
extern int kill(pid_t pid, int32_t sig);

#endif /* !KILL_H */