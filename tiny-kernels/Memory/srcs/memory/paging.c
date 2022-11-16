/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 15:27:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 17:46:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

t_page_directory *kernel_directory = NULL;
t_page_directory *current_directory = NULL;

static t_page *__create_page(uint32_t addr, t_page_directory *kernel_dir)
{
    addr /= PAGE_SIZE;
    uint32_t idx = addr / 1024;
    uint32_t *tmp = NULL;

    kernel_dir->tables[idx] = (t_page_table *)kmalloc_ap(sizeof(t_page_table), &tmp);
    memset(kernel_dir->tables[idx], 0, PAGE_SIZE);
    kernel_dir->tablesPhysical[idx] = (uint32_t)tmp | 0x7;
    return (&kernel_dir->tables[idx]->pages[addr % 1024]);
}

static t_page *__get_page(uint32_t addr, t_page_directory *kernel_dir)
{
    addr /= PAGE_SIZE;
    uint32_t idx = addr / 1024;
    if (kernel_dir->tables[idx])
        return (&kernel_dir->tables[idx]->pages[addr % 1024]);
    return NULL;
}

void init_paging(void)
{

    init_frames();

    printk("Test\n");
    while (1)
        ;

    kernel_directory = (t_page_directory *)kmalloc_a(sizeof(t_page_directory));
    memset(kernel_directory, 0, sizeof(t_page_directory));
    current_directory = kernel_directory;
    printk("Current Directory : 0x%x\n", current_directory);

    printk("Paging : Creating Kernel Directory\n");
    uint32_t i = KHEAP_START;
    while (i < (KHEAP_START + KHEAP_INITIAL_SIZE))
    {
        __create_page(i, kernel_directory);
        i += PAGE_SIZE;
    }
    printk("Create %u pages\n", KHEAP_INITIAL_SIZE / PAGE_SIZE);
    i = 0;
    printk("Create %u pages\n", (placement_address - 0xC0000000 + PAGE_SIZE));
    while (i < (placement_address - 0xC0000000 + PAGE_SIZE))
    {
        t_page *page = __get_page(i, kernel_directory);
        if (!page)
            page = __create_page(i, kernel_directory);
        alloc_frame(page, 0, 0);
        i += PAGE_SIZE;
    }

    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
    {
        t_page *page = __get_page(i, kernel_directory);
        if (!page)
            page = __create_page(i, kernel_directory);
        alloc_frame(page, 0, 0);
    }

    enable_paging(kernel_directory->tablesPhysical);
}