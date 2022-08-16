/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   virtual.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 16:25:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/16 16:25:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

void *vmalloc(size_t size)
{
    (void)size;
    return (NULL);
}

void vfree(void *ptr)
{
    (void)ptr;
}

size_t vsize(void *ptr)
{
    (void)ptr;
    return (0);   
}

void *vbrk(void)
{
    return (NULL);
}