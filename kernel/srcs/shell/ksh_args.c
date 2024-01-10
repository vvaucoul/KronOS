/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_args.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 23:05:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 17:21:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>
#include <shell/ksh_args.h>

int ksh_get_argc(char **argv) {
    int i = 0;

    while (argv[i])
        ++i;
    return (i);
}

static uint32_t __ksh_parse_args_get_argc(const char *cmd) {
    uint32_t argc = 0;
    uint32_t i = 0;

    while (cmd[i]) {
        if (cmd[i] == ' ')
            ++argc;
        ++i;
    }
    return (argc + 1);
}

/**
 * @brief Parse Arguments
 *
 * @param cmd
 * @return const char **
 */
char **ksh_parse_args(char *cmd) {
    char **argv = NULL;

    uint32_t argc = __ksh_parse_args_get_argc(cmd);

    if (argc == 0) {
        return (NULL);
    } else {
        argv = (char **)kmalloc(sizeof(char *) * (argc + 1));
        if (!argv) {
            __WARN("ksh_parse_args: malloc failed", NULL);
            return NULL;
        }
        bzero(argv, sizeof(char *) * (argc + 1));

        char *token;
        int i = 0;
        token = strtok(cmd, " ");
        while (token != NULL) {
            argv[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        argv[i] = NULL;

        return argv;
    }
}
