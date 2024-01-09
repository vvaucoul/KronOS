/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 15:05:27 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 12:53:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_H
#define SOCKET_H

#include <kernel.h>
#include <system/mutex.h>

#define SOCKET_BUFFER_MAX 128
#define MAX_SOCKETS 256

/* Set socket as shared */
#define SOCKET_SHARED_DATA 0x1

typedef uint8_t socket_flags_t;

typedef struct socket {
    int id;
    char buffer[SOCKET_BUFFER_MAX];
    int buffer_len;

    int read_pos;
    int write_pos;

    socket_flags_t flags;

    mutex_t mutex;
} socket_t;

socket_t *socket_create(socket_flags_t flags);
void socket_destroy(socket_t *socket);

extern int socket_send(socket_t *socket, const char *data, int len);
extern int socket_receive(socket_t *socket, char *buffer, int len);

#endif /* !SOCKET_H */