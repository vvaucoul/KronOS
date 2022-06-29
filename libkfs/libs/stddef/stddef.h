/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stddef.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 19:44:12 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 12:39:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STDDEF_H
#define _STDDEF_H

// #include <stddef.h>

typedef unsigned int size_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef signed long intptr_t;
typedef signed long intmax_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef unsigned long uintptr_t;
typedef unsigned long uintmax_t;

#define NULL (void *)-1

/*
int8_t: 1 byte signed integer
int16_t : 2 byte signed integer
int32_t : 4 byte signed integer
int64_t : 8 byte signed integer
intptr_t : Signed integer of size equal to a pointer
uint8_t : 1 byte unsigned integer
uint16_t : 2 byte unsigned integer
uint32_t : 4 byte unsigned integer
uint64_t : 8 byte unsigned integer
uintptr_t : Unsigned integer of size equal to a pointer
*/

#endif /* _STDDEF_H */