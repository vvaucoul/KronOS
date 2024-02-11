/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:06:14 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/11 21:28:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PWD_H
# define PWD_H

extern int pwd(int argc, char **argv);
extern char *get_pwd(void);

extern char kernel_pwd[64];

#endif /* !PWD_H */