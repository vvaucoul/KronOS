/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 18:33:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 16:47:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string.h"

int kstrcmp(const char *s1, const char *s2)
{
    int i;

    if (s1 == NULL || s2 == NULL)
        return (-1);
    i = 0;
    while (s1[i] && s2[i] && s1[i] == s2[i])
        i++;
    return (s1[i] - s2[i]);
}

int kstrncmp(const char *s1, const char *s2, size_t length)
{
    unsigned long int i;

    if (length == 0 || s1 == NULL || s2 == NULL)
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
    if (!str)
        return (NULL);
    while (str[i] && i < length)
        i++;
    str[i] = '\0';
    return (str);
}

char *kstrcat(char *dest, const char *src)
{
    size_t i;
    size_t j;

    i = 0;
    j = 0;
    while (dest[i])
        i++;
    while (src[j])
    {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0';
    return (dest);
}

char *kstrncat(char *dest, const char *src, size_t n)
{
    size_t i;
    size_t j;

    i = 0;
    j = 0;
    while (dest[i])
        i++;
    while (src[j] && j < n)
    {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0';
    return (dest);
}

char *kstrchr(const char *str, int n)
{
    size_t i;

    i = 0;
    if (!str)
        return (NULL);
    while (str[i] && str[i] != n)
        i++;
    if (str[i] == n)
        return ((char *)&str[i]);
    return (NULL);
}