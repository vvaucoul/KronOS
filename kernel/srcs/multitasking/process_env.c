/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:28:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/26 22:47:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <string.h>
#include <memory.h>

int process_init_env(task_t *task) {
    memscpy(task->env.pwd, 64, "/", strlen("/"));
    return (0);
}