/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 18:32:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/27 19:26:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STRING_H
# define _STRING_H

#include "../stddef/stddef.h"

extern size_t kstrlen(const char *str);
extern size_t knbrlen(const int nbr);
extern int kstrcmp(const char *s1, const char *s2);
extern int kstrncmp(const char *s1, const char *s2, size_t length);
extern char *kstrtrunc(char *str, size_t length);

#endif /* _STRING_H */