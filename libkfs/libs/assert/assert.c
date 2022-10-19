/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assert.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 16:57:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/18 16:21:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "assert.h"

assert_t __assert(__assert_t condition, const char *file, const char *function, uint32_t line)
{
    if (condition == E_ASSERT_SUCCESS)
    {
        kprintf(COLOR_RED "Asssertion failed" COLOR_END "\nFile: %s:" COLOR_RED "%u:%s" COLOR_END "\n", file, line, function);
        while (1)
            ;
        return (1);
    }
    else
        return (0);
}