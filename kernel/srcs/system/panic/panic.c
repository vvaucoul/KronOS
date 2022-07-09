/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:27:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/09 12:28:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/panic.h>

extern void kernel_panic(const char *str)
{
    kprintf(COLOR_END "PANIC: " COLOR_RED "%s\n" COLOR_END, str);
    while (1)
        ;
}
