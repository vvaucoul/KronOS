/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kconvert.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 19:52:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/23 19:55:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libkfs.h"

static int n_lenght(int n)
{
    int nsize;

    nsize = 0;
    if (n > -2147483648 && n < 0)
    {
        ++nsize;
        n *= -1;
    }
    else if (n == 0)
        return (1);
    else if (n == -2147483648)
        return (11);
    while (n >= 1)
    {
        n /= 10;
        ++nsize;
    }
    return (nsize);
}

int kitoa(int nbr, char str[__KITOA_BUFFER_LENGTH__])
{
    int i;
    int size;
    int isneg;

    size = n_lenght(nbr);
    if (!str)
        return (1);
    if ((isneg = ((nbr >= -2147483647 && nbr < 0) ? 1 : 0)) == 1)
        nbr *= -1;
    i = (isneg ? 1 : 1);
    while (size - i >= 0)
    {
        str[size - i] = nbr % 10 + 48;
        nbr /= 10;
        ++i;
    }
    if (isneg)
        str[0] = '-';
    str[size] = '\0';
    return (0);
}

int katoi(const char *str)
{
    int res = 0;
    int sign = 1;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    while (*str)
    {
        res = res * 10 + (*str - '0');
        str++;
    }
    return (res * sign);
}