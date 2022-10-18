/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assert.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 16:57:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/17 17:04:38 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "assert.h"

assert_t __assert(__assert_t condition, const char *file, const char *function, uint32_t line)
{
    if (condition == E_ASSERT_FAILURE)
    {
        kprintf("%s:%u %s: Assertion failed", file, line, function);
        return (1);
    }
    else
        return (0);
}