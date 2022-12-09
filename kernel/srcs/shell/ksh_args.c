/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_args.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 23:05:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 00:26:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh_args.h>
#include <memory/kheap.h>

static uint32_t __ksh_get_argc(char *cmd)
{
    uint32_t argc = 0;
    uint32_t len = strlen(cmd);

    for (uint32_t i = 0; i < len; i++)
    {
        if (cmd[i] == ' ')
        {
            argc++;
        }
    }
    return (argc);
}

static char *__ksh_get_nex_arg(char *cmd, uint32_t index)
{
    while (cmd[index] != '\0')
    {
        if (cmd[index] != ' ')
            break;
        index++;
    }
    if (cmd[index] == '\0')
        return (NULL);
    else
        return (strtrtc(cmd + index, ' '));
}

void ksh_free_args(const ksh_args_t *args)
{
    if (args == NULL)
        return;
    if (args->cmd != NULL)
        kfree(args->cmd);
    if (args->argv != NULL)
    {
        for (uint32_t i = 0; i < args->argc; i++)
            kfree(args->argv[i]);
        kfree(args->argv);
    }
    kfree((void *)args);
}

/**
 * @brief Parse Arguments
 *
 * @param cmd
 * @return const ksh_args_t*
 */
const ksh_args_t *ksh_parse_args(char *cmd)
{
    ksh_args_t *args;

    if (!(args = (ksh_args_t *)kmalloc(sizeof(ksh_args_t))))
        return (NULL);

    args->cmd = NULL;
    args->argc = __ksh_get_argc(cmd);
    args->argv = (char **)kmalloc(sizeof(char *) * (args->argc + 1));

    bzero(args->argv, sizeof(char *) * (args->argc + 1));

    if (args->argv == NULL)
        return (NULL);

    args->cmd = strtrtc(cmd, ' ');

    uint32_t i = strlen(args->cmd) + 1;

    char *current_arg = NULL;

    uint32_t j = 0;
    do
    {
        current_arg = __ksh_get_nex_arg(cmd, i);
        if (current_arg == NULL)
            break;
        else
            args->argv[j++] = current_arg;
        i += strlen(current_arg) + 1;
    } while (current_arg != NULL);

    return (args);
}

/*******************************************************************************
 *                           USEFULL ARGS FUNCTIONS                            *
 ******************************************************************************/

/**
 * @brief Get argument at index
 *
 * @param args
 * @param index
 * @return char*
 */
char *ksh_get_arg(const ksh_args_t *args, const uint8_t index)
{
    if (index >= args->argc)
        return (NULL);
    return (args->argv[index]);
}

/**
 * @brief Get command
 *
 * @param args
 * @return char*
 */
char *ksh_get_cmd(const ksh_args_t *args)
{
    return (args->cmd);
}

/**
 * @brief Get argument count
 *
 * @param args
 * @return uint8_t
 */
uint8_t ksh_get_argc(const ksh_args_t *args)
{
    return (args->argc);
}

/**
 * @brief Get argument vector
 *
 * @param args
 * @return char**
 */
char **ksh_get_argv(const ksh_args_t *args)
{
    return (args->argv);
}