/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physical.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 16:23:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/17 15:08:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

void *kmalloc(size_t size)
{
    (void)size;
    return (__request_new_page(size));
    return (NULL);
}

void kfree(void *ptr)
{
    (void)ptr;
}

size_t ksize(void *ptr)
{
    (void)ptr;
    return (0);
}

void *kbrk(void)
{
    return (NULL);

}