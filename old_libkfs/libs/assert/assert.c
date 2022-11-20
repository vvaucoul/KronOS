/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assert.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 16:57:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/19 12:49:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "assert.h"

assert_t __assert(__assert_t condition, const char *file, const char *function, uint32_t line)
{
    if (condition == E_ASSERT_FAILURE)
    {
        kprintf(_RED "Asssertion failed: " _END "%s: - ["_RED
                     ".%u"_END
                     "]: "_RED
                     "%s"
                     "" _END "\n",
                file, line, function);
        while (1)
            ;
        return (1);
    }
    else
        return (0);
}