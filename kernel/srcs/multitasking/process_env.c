/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:28:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/28 11:31:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <string.h>

int process_init_env(task_t *task) {
    memscpy(task->env.pwd, 64, "/", strlen("/"));
    return (0);
}