/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ps.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 14:18:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:59:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ps.h>
#include <multitasking/process.h>

// Todo: implement ps command with args
static void __ps(void) {
    print_all_tasks();
}

void ps(const ksh_args_t *args) {
    __ps();
    __UNUSED(args);
}