/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 12:56:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/24 17:22:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "convert.h"
#include "../string/string.h"
#include "../../includes/kprintf.h"

static uint32_t __get_nbr_length(int64_t nbr)
{
    uint32_t length = 0;
    while (nbr)
    {
        nbr /= 10;
        ++length;
    }
    return (length);
}

uint32_t kuitoa_base(uint32_t nbr, int base, char str[__KITOA_BUFFER_LENGTH__])
{
    int i;
    int size;

    size = __get_nbr_length(nbr);
    if (!str)
        return (1);
    i = 3;
    while (size - i >= 0)
    {
        str[size - i] = __ASCII_BASE__[nbr % base];
        nbr /= base;
        ++i;
    }
    str[size] = '\0';
    return (0);
}

int kitoa_base(int nbr, int base, char str[__KITOA_BUFFER_LENGTH__])
{
    int i;
    int size;
    int isneg;

    size = __get_nbr_length(nbr);
    if (!str)
        return (1);
    if ((isneg = ((nbr >= -2147483647 && nbr < 0) ? 1 : 0)) == 1)
        nbr *= -1;
    i = (isneg ? 1 : 1);
    while (size - i >= 0)
    {
        str[size - i] = __ASCII_BASE__[nbr % base];
        nbr /= base;
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

/*******************************************************************************
 *                             EXTERN C FUNCTIONS                              *
 ******************************************************************************/

int kitoa(int nbr, char str[__KITOA_BUFFER_LENGTH__])
{
    int i;
    bool is_neg;

    if (!str)
        return (1);
    is_neg = ((nbr >= -2147483647 && nbr < 0) ? 1 : 0);
    if (is_neg)
        nbr *= -1;
    i = (is_neg ? 1 : 1);
    while (nbr)
    {
        str[__KITOA_BUFFER_LENGTH__ - i] = nbr % 10 + 48;
        nbr /= 10;
        ++i;
    }
    if (is_neg)
        str[0] = '-';
    str[__KITOA_BUFFER_LENGTH__ - i] = '\0';
    return (0);
}

int kuitoa(uint32_t nbr, char str[__KITOA_BUFFER_LENGTH__])
{
    int i = 1;
    int size = 0;

    if (str == NULL)
        return (1);
    else
    {
        uint32_t __tmp_nbr = nbr;
        while (__tmp_nbr)
        {
            __tmp_nbr /= 10;
            ++size;
        }
    }
    while (nbr)
    {
        str[size - i] = nbr % 10 + 48;
        nbr /= 10;
        ++i;
    }
    str[size] = '\0';
    return (0);
}

// int kultoa(uint64_t nbr, char str[__KITOA_BUFFER_LENGTH__])
// {
//     return (ktoa(nbr, str));
// }

// int kstoa(int8_t nbr, char str[__KITOA_BUFFER_LENGTH__])
// {
//     return (ktoa(nbr, str));
// }

// int kustoa(uint8_t nbr, char str[__KITOA_BUFFER_LENGTH__])
// {
//     return (ktoa(nbr, str));
// }