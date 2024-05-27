/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:28:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 17:08:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <multitasking/process.h>

int process_init_env(task_t *task) {
    memscpy(task->env.pwd, 64, "/", strlen("/"));
    return (0);
}