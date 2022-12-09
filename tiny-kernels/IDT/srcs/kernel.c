/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 00:07:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 16:35:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include "gdt.h"
#include "idt.h"

int kmain(void)
{
    terminal_initialize();
    gdt_init();
    idt_install();
    terminal_writestring("Hello, kernel World!\n");

    while (1)
        ;

    return (0);
}