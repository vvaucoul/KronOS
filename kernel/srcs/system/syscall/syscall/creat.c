/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 23:32:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 19:37:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>
#include <system/fs/open.h>

int sys_creat(const char *path, mode_t mode) {
    return (sys_open(path, O_CREAT | O_WRONLY | O_TRUNC, mode));
}