/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:42:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/17 15:07:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <kernel.h>
#include "paging.h"
#include "alloc.h"

#define MEMORY_START 0x100000
#define PAGE_SIZE 4096

// #define MEMORY_MAX 4294967296
#define MEMORY_MAX (4 * 1024 * 1024 * 1024)
#define MAX_PAGES (MEMORY_MAX / PAGE_SIZE)

typedef struct s_memory_list
{
    PageTable *page;
    struct s_memory_list *next;
    struct s_memory_list *prev;
} t_memory_list;

#define Page t_page

#define Memory_List t_memory_list
#ifndef __MEMORY_LIST__
#define __MemorySystem Memory_List
#define __MEMORY_LIST__
#endif

extern __MemorySystem *__mem_root;

extern void init_kernel_memory(void);
extern size_t __nb_pages;

#endif