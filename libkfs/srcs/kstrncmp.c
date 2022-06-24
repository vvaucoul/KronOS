/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kstrncmp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 15:42:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/22 18:39:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libkfs.h"

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