/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 00:07:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 00:10:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include "gdt.h"

int kmain(void)
{
    terminal_initialize();
    gdt_init();
    terminal_writestring("Hello, kernel World!\n");

    return (0);
}