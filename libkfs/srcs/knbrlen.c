/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   knbrlen.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/21 21:24:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/22 18:38:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libkfs.h"

size_t knbrlen(const int nbr)
{
    size_t i = 0;
    int nb = nbr;

    if (nbr < 0)
        i++;
    while (nb != 0)
    {
        nb /= 10;
        i++;
    }
    return (i);
}