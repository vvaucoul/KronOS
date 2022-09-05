/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_builtins.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 01:10:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/05 02:13:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_BUILTINS_H
#define KSH_BUILTINS_H

#define __NB_BUILTINS_ 0x09
#define __BUILTINS_MAX_NAMES 0x04
#define __BUILTINS_MAX_NAME_LENGTH 0x80

typedef struct s_ksh_builtins
{
    char names[__BUILTINS_MAX_NAMES][__BUILTINS_MAX_NAME_LENGTH];
    void (*function)(void);
} t_ksh_builtins;

#define KshBuiltins t_ksh_builtins

union u_ksh_builtins
{
    char *names [__BUILTINS_MAX_NAMES];
    void (*function)(void);
};

extern union u_ksh_builtins __unions_ksh_builtins[__NB_BUILTINS_];
extern KshBuiltins __ksh_builtins[__NB_BUILTINS_];

extern void __ksh_init_builtins(void);
extern void __ksh_execute_builtins(const char *name);

#endif /* !KSH_BUILTINS_H */