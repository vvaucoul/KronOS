/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ipc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 14:30:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 14:49:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IPC_H
# define IPC_H

#define IPC_MSG_MAX 32
#define IPC_QUEUE_MAX ((uint16_t)128)

typedef struct ipc_msg {
    pid_t from;
    pid_t to;
    char message[IPC_MSG_MAX];
} ipc_msg_t;

extern int ipc_send(pid_t to, const char *message);
extern int ipc_receive(pid_t from, char *buffer);

#endif /* !IPC_H */