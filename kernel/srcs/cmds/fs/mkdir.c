/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mkdir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 17:05:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 17:32:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>

int mkdir(int argc, char **argv) {
    if (argc < 2) {
        printk("Usage: mkdir <directory>\n");
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        int status = sys_mkdir(argv[i], 0755);
        if (status != 0) {
            printk("mkdir: cannot create directory `%s`\n", argv[i]);
            return 1;
        }
    }

    return 0;
}