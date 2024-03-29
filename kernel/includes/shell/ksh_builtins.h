/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_builtins.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 01:10:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 00:17:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_BUILTINS_H
#define KSH_BUILTINS_H

#include <shell/ksh_args.h>

#define __NB_BUILTINS_ 0x0E
#define __BUILTINS_MAX_NAMES 0x04
#define __BUILTINS_MAX_NAME_LENGTH 0x80

#define __BUILTINS_MAX_PARAMS 0x05

typedef struct s_ksh_parameters
{
    char *params[__BUILTINS_MAX_PARAMS];
} ksh_parameters_t;

typedef struct s_ksh_builtins
{
    char names[__BUILTINS_MAX_NAMES][__BUILTINS_MAX_NAME_LENGTH];
    ksh_cmd_t function;
    // void (*function)(void);
} t_ksh_builtins;

#define KshBuiltins t_ksh_builtins

extern KshBuiltins __ksh_builtins[__NB_BUILTINS_];

extern void __ksh_init_builtins(void);
extern void __ksh_execute_builtins(const ksh_args_t *arg);

#endif /* !KSH_BUILTINS_H */