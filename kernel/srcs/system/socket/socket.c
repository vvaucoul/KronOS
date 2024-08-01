/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 15:05:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:11:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/mm.h>
#include <mm/shared.h>
#include <multitasking/process.h>
#include <system/socket.h>

static socket_t *sockets[MAX_SOCKETS] = {NULL};
static mutex_t socket_mutex = (mutex_t){
    .state = MUTEX_UNLOCKED,
    .owner = 1,
    .wait_queue = NULL,
};

socket_t *socket_create(socket_flags_t flags) {
    mutex_lock(&socket_mutex);
    socket_t *socket = NULL;

    if (flags & SOCKET_SHARED_DATA) {
        socket = kmalloc_shared(sizeof(socket_t));
    } else {
        socket = kmalloc(sizeof(socket_t));
    }

    if (!(socket)) {
        __THROW("Failed to create socket", NULL);
    }

    socket->id = getpid();
    socket->buffer_len = 0;
    socket->read_pos = 0;
    socket->write_pos = 0;
    socket->flags = flags;

    init_mutex(&socket->mutex);

    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i] == NULL) {
            sockets[i] = socket;
            break;
        }
    }
    mutex_unlock(&socket_mutex);
    return (socket);
}

void socket_destroy(socket_t *socket) {
    mutex_lock(&socket_mutex);
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i] == socket) {
            sockets[i] = NULL;
            break;
        }
    }
    if (socket->flags & SOCKET_SHARED_DATA) {
        kfree_shared(socket);
    } else {
        kfree(socket);
    }
    mutex_unlock(&socket_mutex);
}

int socket_send(socket_t *socket, const char *data, int len) {
    mutex_lock(&socket->mutex);

    int space_available = SOCKET_BUFFER_MAX - socket->write_pos;
    if (len > space_available) {
        len = space_available;
    }

    memcpy(socket->buffer + socket->write_pos, data, len);
    socket->write_pos = (socket->write_pos + len) % SOCKET_BUFFER_MAX;

    mutex_unlock(&socket->mutex);
    return (len);
}

int socket_receive(socket_t *socket, char *buffer, int len) {
    mutex_lock(&socket->mutex);

    int data_available = socket->write_pos - socket->read_pos;
    if (len > data_available) {
        len = data_available;
    }

    memcpy(buffer, socket->buffer + socket->read_pos, len);
    socket->read_pos = (socket->read_pos + len) % SOCKET_BUFFER_MAX;

    mutex_unlock(&socket->mutex);
    return (len);
}