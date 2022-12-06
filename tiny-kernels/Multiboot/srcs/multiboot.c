/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 19:12:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 12:55:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "multiboot.h"

MultibootInfo *__multiboot_info;

int multiboot_check(void)
{
    if (CHECK_FLAG(__multiboot_info->flags, 0) == 0)
    {
        return (1);
    }
    if (CHECK_FLAG(__multiboot_info->flags, 1) == 0)
    {
        return (1);
    }
    terminal_writestring("Multiboot info OK\n");
    return (0);
}

int multiboot_init(uint32_t *mboot_ptr)
{
    if (mboot_ptr == NULL)
        return (1);
    else
        __multiboot_info = (MultibootInfo *)mboot_ptr;
    return (0);
}