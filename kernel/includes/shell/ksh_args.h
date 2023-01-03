/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_args.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 23:06:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 00:21:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_ARGS_H
#define KSH_ARGS_H

#include <kernel.h>

/*******************************************************************************
 *                             ARGUMENTS STRUCTURE                             *
 ******************************************************************************/

typedef struct s_ksh_args
{
    char *cmd;

    uint8_t argc;
    char **argv;
} ksh_args_t;

/*******************************************************************************
 *                         GLOBAL FUNCTION PROTOTYPES                          *
 ******************************************************************************/

typedef int (*ksh_cmd_t)(const ksh_args_t *args) __attribute__((__nonnull__(1)));

/*******************************************************************************
 *                               ARGS FUNCTIONS                                *
 ******************************************************************************/

/* Parse arguments */
extern const ksh_args_t *ksh_parse_args(char *cmd);
extern void ksh_free_args(const ksh_args_t *args);

/* Usefull functions */
extern char *ksh_get_arg(const ksh_args_t *args, const uint8_t index);
extern char *ksh_get_cmd(const ksh_args_t *args);
extern uint8_t ksh_get_argc(const ksh_args_t *args);
extern char **ksh_get_argv(const ksh_args_t *args);

#endif /* !KSH_ARGS_H */