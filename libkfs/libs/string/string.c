/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 18:33:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/27 19:26:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string.h"

int kstrcmp(const char *s1, const char *s2)
{
    int i;

    i = 0;
    while (s1[i] && s2[i] && s1[i] == s2[i])
        i++;
    return (s1[i] - s2[i]);
}

int kstrncmp(const char *s1, const char *s2, size_t length)
{
    unsigned long int i;

    if (length == 0)
        return (0);
    i = 0;
    while (i < length && (unsigned char)s1[i] == (unsigned char)s2[i])
    {
        if (((unsigned char)s1[i] == 0 &&
             (unsigned char)s2[i] == 0) ||
            i == (length - 1))
            return (0);
        ++i;
    }
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

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

char *kstrtrunc(char *str, size_t length)
{
    size_t i;

    i = 0;
    while (str[i] && i < length)
        i++;
    str[i] = '\0';
    return (str);
}