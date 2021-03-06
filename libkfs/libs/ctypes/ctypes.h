/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctypes.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 11:58:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 19:58:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CTYPES_H
#define _CTYPES_H

#include "../stdbool/stdbool.h"

extern bool isalnum(int c);
extern bool isalpha(int c);
extern bool isblank(int c);
extern bool iscntrl(int c);
extern bool isdigit(int c);
extern bool isgraph(int c);
extern bool islower(int c);
extern bool isprint(int c);
extern bool ispunct(int c);
extern bool isupper(int c);
extern bool isxdigit(int c);

extern void tolower(int c);
extern void toupper(int c);

#endif /* _CTYPES_H */