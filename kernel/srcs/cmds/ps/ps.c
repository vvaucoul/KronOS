/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ps.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 14:18:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 17:13:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ps.h>
#include <multitasking/process.h>

// Todo: implement ps command with args
static void __ps(void) {
    print_all_tasks();
}

void ps(int argc, char **argv) {
    __ps();
    __UNUSED(argc);
    __UNUSED(argv);
}