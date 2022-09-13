/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stddef.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 19:44:12 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 16:48:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STDDEF_H
#define _STDDEF_H

typedef unsigned int size_t;
typedef int ssize_t;
typedef unsigned char uchar_t;

typedef unsigned int hex_t;
typedef unsigned long long hex64_t;

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

#define __UNUSED__(x) ((void)(x))
#define ATTR_UNUSED __attribute__((unused))
#define ATTR_NORETURN __attribute__((noreturn))
#define private static
#define public extern

#endif /* _STDDEF_H */