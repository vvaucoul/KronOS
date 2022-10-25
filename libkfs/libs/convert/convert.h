/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 12:56:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/24 17:18:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CONVERT_H
#define _CONVERT_H

#include "../stddef/stddef.h"
#include "../stdbool/stdbool.h"

#define __KITOA_BUFFER_LENGTH__ 11
#define __ASCII_BASE__ "0123456789ABCDEF"

/*
** Convert Int To Ascii
*/
extern int kitoa(int nbr, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Unsigned Int To Ascii
*/
extern int kuitoa(uint32_t nbr, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Int to Ascii with base
*/
extern int kitoa_base(int nbr, int base, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Unsigned Int to Ascii with base
*/
extern uint32_t kuitoa_base(uint32_t nbr, int base, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Unsigned long to Ascii
*/
extern int kultoa(uint64_t nbr, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Short to Ascii
*/
extern int kstoa(int8_t nbr, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Unsigned Short to Ascii
*/
extern int kustoa(uint8_t nbr, char str[__KITOA_BUFFER_LENGTH__]);

/*
** Convert Ascii to Int
*/
extern int katoi(const char *str);

/*
** Convert Float to Ascii
*/
extern void kftoa(char *buffer, float value);

#endif /* !_CONVERT_H */
