/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_fs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:28:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 17:37:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process_fs.h>

int process_fs_init(task_t *task) {
    task->env.current_directory = fs_root;
    return (0);
}