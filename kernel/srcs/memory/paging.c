/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:46:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/29 15:42:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

// void *get_new_page()
// {
//     void *page = (void *)MEMORY_START;
//     __nb_pages++;
//     return page;
// }

void *__request_new_page(size_t size)
{
    // return (0x00080000);
    // return (__mem_root->page);
    (void)size;
    return (0);
}