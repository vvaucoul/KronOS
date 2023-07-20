/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 15:05:27 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 15:06:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_H
# define SOCKET_H

# include <kernel.h>

#define SOCKET_BUFFER_MAX 128

typedef struct socket {
    int id;
    char buffer[SOCKET_BUFFER_MAX];
    int buffer_len;
} socket_t;

socket_t *socket_create(void);
void socket_destroy(socket_t *socket);

extern int socket_send(socket_t *socket, const char *data, int len);
extern int socket_receive(socket_t *socket, char *buffer, int len);

#endif /* !SOCKET_H */