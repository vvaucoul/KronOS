/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/19 14:58:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/19 20:03:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

int kmain()
{
    terminal_initialize();
    terminal_clear_screen();
    terminal_writestring("Hello, kernel World!\n");
    while (1)
           ;
}