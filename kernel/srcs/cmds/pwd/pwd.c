/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 10:48:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 12:11:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/pwd.h>
#include <multitasking/process.h>

char kernel_pwd[64] = {0};

int pwd(int argc, char **argv) {

    if (argc > 1) {
        __WARND("pwd: too many arguments");
        return (1);
    } else {
        task_t *task = get_task(getpid());

        if (task == NULL) {
            printk("%s\n", kernel_pwd);
        } else {
            printk("%s\n", task->env.pwd);
        }
        return (0);
    }
}