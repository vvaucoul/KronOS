/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:53:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/16 16:36:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

__MemorySystem *root;
size_t __nb_pages;

void init_kernel_memory(void)
{
    root = NULL;
    __nb_pages = 0;

    
    // Test Debug
    Page page;

    page.p = 1;
    page.rw = 1;
    page.us = 1;
    page.pwt = 1;
    page.pcd = 1;
    page.a = 0;
    page.d = 0;
    page.pat = 0;
    page.g = 1;
    page.avail = 0;
    page.pfa = 0;


    enable_paging(&page);
    //enable_paging(&root);
}