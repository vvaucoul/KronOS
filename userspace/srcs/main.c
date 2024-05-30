/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 20:23:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/30 12:54:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/user_syscall.h>
#include <userspace.h>

int switch_user_mode_start(void) {
    write("Hello from userspace\n");
    while (1) {
    }
    return (0);
}
