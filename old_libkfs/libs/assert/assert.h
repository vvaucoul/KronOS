/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assert.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 16:48:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/17 02:18:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _ASSERT_H
#define _ASSERT_H

#include <kprintf.h>
#include <stddef.h>

typedef unsigned short __assert_t;

#define E_ASSERT_FAILURE 0
#define E_ASSERT_SUCCESS 1

#define assert_t __assert_t

#ifndef __ASSERT
#define __ASSERT
extern assert_t __assert(__assert_t condition, const char *file, const char *function, uint32_t line);
#define assert(x) __assert(((__assert_t)(x)), __FILE__, __FUNCTION__, __LINE__)
#endif

#endif /* !_ASSERT_H */