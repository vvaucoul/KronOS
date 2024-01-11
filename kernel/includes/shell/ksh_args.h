/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_args.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 23:06:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 11:37:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_ARGS_H
#define KSH_ARGS_H

#include <kernel.h>

#define ksh_args (int argc, char **argv)

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 ARGS FUNCTIONS                                 ||
// ! ||--------------------------------------------------------------------------------||

extern int ksh_get_argc(char **argv);
extern char **ksh_parse_args(char *cmd);

extern int ksh_has_arg(int argc, char **argv, char *arg);
extern int ksh_contain_arg(int argc, char **argv, char *arg);
extern void *ksh_get_arg_value(int argc, char **argv, char *arg, char cmp);

#endif /* !KSH_ARGS_H */