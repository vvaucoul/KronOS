/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ps.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 14:18:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 08:45:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ps.h>
#include <multitasking/process.h>

#include <macros.h>

// Todo: implement ps command with args
static void __ps(void) {
    print_all_tasks();
}

void ps(__unused__ int argc, __unused__ char **argv) {
    __ps();
}