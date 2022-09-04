/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 18:33:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 23:57:07 by vvaucoul         ###   ########.fr       */
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

char *kstrrchr(const char *str, int n)
{
    size_t i;

    i = 0;
    if (!str)
        return (NULL);
    while (str[i])
        i++;
    while (i > 0 && str[i] != n)
        i--;
    if (str[i] == n)
        return ((char *)&str[i]);
    return (NULL);
}

bool kccof(const char c, const char *str)
{
    size_t i;

    i = 0;
    if (!str)
        return (false);
    while (str[i])
    {
        if (str[i] == c)
            return (true);
        i++;
    }
    return (false);
}

bool kacof(const char *str, const char *chars)
{
    size_t i;

    i = 0;
    if (!str)
        return (false);
    while (str[i])
    {
        if (kccof(str[i], chars))
            return (true);
        i++;
    }
    return (false);
}

void kstrmoveoffset(char *str, size_t offset)
{
    if (!str)
        return;
    for (size_t j = 0; j < offset; j++)
    {
        for (size_t i = 0; str[i] && i < kstrlen(str); i++)
            str[i] = str[i + 1];
        str[kstrlen(str)] = '\0';
    }
}

char *kstrclr(char *new_str, char *str)
{
    if (str == NULL)
        return (NULL);
    kbzero(new_str, kstrlen(new_str));

    size_t i = 0;
    /* Skip all firsts spaces */
    while (str[i] && str[i] == ' ')
        i++;

    size_t j = 0;

    /* Skip all spaces in the string */
    while (str[i])
    {
        if (str[i] == ' ')
        {
            while (str[i] && str[i] == ' ')
                i++;
            if (str[i] == '\0')
                break;
            new_str[j] = ' ';
            j++;
        }
        new_str[j] = str[i];
        i++;
        j++;
    }
    new_str[j] = '\0';
    return (new_str);
}