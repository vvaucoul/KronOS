/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmemory.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 11:56:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 18:26:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libkfs.h"

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