/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 15:05:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 15:07:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/socket.h>
#include <multitasking/process.h>
#include <memory/memory.h>

socket_t *socket_create(void) {
    socket_t *socket = kmalloc(sizeof(socket_t));
    socket->id = getpid();
    socket->buffer_len = 0;
    return socket;
}

void socket_destroy(socket_t *socket) {
    kfree(socket);
}

int socket_send(socket_t *socket, const char *data, int len) {
    if (len > SOCKET_BUFFER_MAX - socket->buffer_len) {
        __THROW("Socket buffer is full", 1);
    }

    memcpy(socket->buffer + socket->buffer_len, data, len);
    socket->buffer_len += len;

    return (len);
}

int socket_receive(socket_t *socket, char *buffer, int len) {
    if (len > socket->buffer_len) {
        len = socket->buffer_len;
    }

    memcpy(buffer, socket->buffer, len);
    memmove(socket->buffer, socket->buffer + len, socket->buffer_len - len);
    socket->buffer_len -= len;

    return (len);
}