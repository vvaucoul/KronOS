/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmemory.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 11:56:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 16:44:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string.h"

void *kbzero(void *ptr, size_t len)
{
    unsigned char *tmp = ptr;

    while (len)
    {
        *(tmp++) = 0;
        --len;
    }
    return (ptr);
}

void *kmemset(void *ptr, int value, size_t length)
{
    unsigned char *tmp = (unsigned char *)ptr;

    while (length--)
        *(tmp++) = (unsigned char)value;
    return (ptr);
}

void *kmemcpy(void *dst, const void *src, size_t length)
{
    unsigned char *tmp = (unsigned char *)dst;
    const unsigned char *tmp2 = (const unsigned char *)src;

    while (length--)
        *(tmp++) = *(tmp2++);
    return (dst);
}

void *kmemmove(void *dst, const void *src, size_t length)
{
    unsigned char *tmp = (unsigned char *)dst;
    const unsigned char *tmp2 = (const unsigned char *)src;

    if (tmp < tmp2)
        while (length--)
            *(tmp++) = *(tmp2++);
    else
        while (length--)
            *(tmp + length) = *(tmp2 + length);
    return (dst);
}

void *kmemchr(const void *s, int c, size_t n)
{
    unsigned char *tmp = (unsigned char *)s;

    while (n--)
        if (*tmp == (unsigned char)c)
            return (tmp);
        else
            ++tmp;
    return (NULL);
}

int kmemcmp(const void *s1, const void *s2, size_t n)
{
    unsigned char *tmp = (unsigned char *)s1;
    unsigned char *tmp2 = (unsigned char *)s2;

    while (n--)
        if (*tmp != *tmp2)
            return (*tmp - *tmp2);
        else
            ++tmp, ++tmp2;
    return (0);
}