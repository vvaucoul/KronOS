/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ipc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 14:30:24 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 22:16:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <multitasking/process.h>
#include <system/ipc.h>

#include <string.h>

static ipc_msg_t ipc_queue[IPC_QUEUE_MAX];
static uint16_t ipc_queue_start = 0;
static uint16_t ipc_queue_end = 0;

int ipc_send(pid_t to, const char *message) {
    if ((ipc_queue_end + 1) % IPC_QUEUE_MAX == ipc_queue_start) {
        __THROW("IPC queue is full", 1);
    }

    ipc_queue[ipc_queue_end].from = getpid();
    ipc_queue[ipc_queue_end].to = to;
    strncpy(ipc_queue[ipc_queue_end].message, message, IPC_MSG_MAX);

    ipc_queue_end = (ipc_queue_end + 1) % IPC_QUEUE_MAX;
    return (0);
}

int ipc_receive(pid_t from, char *buffer) {
    for (uint16_t i = ipc_queue_start; i != ipc_queue_end; i = (i + 1) % IPC_QUEUE_MAX) {
        if (ipc_queue[i].to == getpid() && ipc_queue[i].from == from) {
            strncpy(buffer, ipc_queue[i].message, IPC_MSG_MAX);

            // Remove the message from the queue
            ipc_queue_start = (ipc_queue_start + 1) % IPC_QUEUE_MAX;

            return (strlen(buffer));
        }
    }

    // No message found from the specified sender
    buffer[0] = '\0';
    return (0);
}