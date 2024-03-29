/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uid.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 14:20:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 14:30:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/uid.h>

uid_t getuid(void)
{
    return (get_current_task()->owner);
}

uid_t geteuid(void)
{
    return (get_current_task()->effective_owner);
}

void setuid(uid_t uid)
{
    get_current_task()->owner = uid;
}

void seteuid(uid_t uid)
{
    get_current_task()->effective_owner = uid;
}