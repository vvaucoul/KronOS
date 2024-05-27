/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 10:48:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 17:12:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/pwd.h>
#include <multitasking/process.h>
#include <syscall/getcwd.h>

int pwd(int argc, __unused__ char **argv) {

    if (argc > 1) {
        __WARND("pwd: too many arguments");
        return (1);
    } else {
        char buf[PATH_MAX] = {0};

        getcwd(buf, PATH_MAX);
        printk("%s\n", buf);
    }
    return (0);
}