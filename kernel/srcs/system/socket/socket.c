/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 15:05:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 12:04:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>
#include <system/socket.h>

#include <system/mutex.h>

static mutex_t socket_mutex = (mutex_t){
    .state = MUTEX_UNLOCKED,
    .owner = 0,
    .wait_queue = NULL,
};

socket_t *socket_create(void) {
    socket_t *socket = kmalloc(sizeof(socket_t));
    socket->id = getpid();
    socket->buffer_len = 0;

    init_mutex(&socket_mutex);

    return (socket);
}

void socket_destroy(socket_t *socket) {
    mutex_lock(&socket_mutex);
    kfree(socket);
    mutex_unlock(&socket_mutex);
}

int socket_send(socket_t *socket, const char *data, int len) {
    mutex_lock(&socket_mutex);
    if (len > SOCKET_BUFFER_MAX - socket->buffer_len) {
        __THROW("Socket buffer is full", 1);
    }

    memcpy(socket->buffer + socket->buffer_len, data, len);
    socket->buffer_len += len;

    mutex_unlock(&socket_mutex);
    return (len);
}

int socket_receive(socket_t *socket, char *buffer, int len) {
    mutex_lock(&socket_mutex);
    if (len > socket->buffer_len) {
        len = socket->buffer_len;
    }

    memcpy(buffer, socket->buffer, len);
    memmove(socket->buffer, socket->buffer + len, socket->buffer_len - len);
    socket->buffer_len -= len;

    mutex_unlock(&socket_mutex);
    return (len);
}