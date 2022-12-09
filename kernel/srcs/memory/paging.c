/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 23:03:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>
#include <memory/frames.h>
#include <memory/kheap.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;
bool paging_enabled = false;

void *get_physical_address(void *addr)
{
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    uint32_t offset = (uint32_t)addr % PAGE_SIZE;
    page_t *p = get_page(page, kernel_directory);

    if (!p)
        return (NULL);

    return (void *)((p->frame * PAGE_SIZE) + offset - KHEAP_START);
}

void *get_virtual_address(void *addr)
{
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    uint32_t offset = (uint32_t)addr % PAGE_SIZE;
    page_t *p = get_page(page, kernel_directory);
    if (p->frame != 0)
        return ((void *)(p->frame * PAGE_SIZE + offset));
    return (NULL);
}

page_t *create_page(uint32_t address, page_directory_t *dir)
{
    address /= PAGE_SIZE;
    uint32_t table_idx = address / 1024;

    dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
    memset(dir->tables[table_idx], 0, PAGE_SIZE);
    dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    return (&dir->tables[table_idx]->pages[address % 1024]);
}

page_t *get_page(uint32_t address, page_directory_t *dir)
{
    address /= PAGE_SIZE;
    uint32_t table_idx = address / PAGE_TABLE_SIZE;
    if (dir->tables[table_idx])
    {
        return (&dir->tables[table_idx]->pages[address % 1024]);
    }
    return (NULL);
}

page_t *create_user_page(uint32_t address, uint32_t end_addr, page_directory_t *dir)
{
    // TODO: Copies kernel space into a new page directory and makes the range from address to end_addr user accessible.

    address /= PAGE_SIZE;
    uint32_t table_idx = address / 1024;

    dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
    memset(dir->tables[table_idx], 0, PAGE_SIZE);
    dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    // for (uint32_t i = address; i < end_addr; i++)
    // {
    //     dir->tables[table_idx]->pages[i % 1024].frame = i;
    //     dir->tables[table_idx]->pages[i % 1024].present = 1;
    //     dir->tables[table_idx]->pages[i % 1024].rw = 1;
    //     dir->tables[table_idx]->pages[i % 1024].user = 1;
    // }

    __UNUSED(end_addr);
    return (&dir->tables[table_idx]->pages[address % 1024]);
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3" ::"r"(dir->physicalAddr));
    // enable_paging((page_directory_t *)&dir->tablesPhysical);
}

void init_paging(void)
{
    init_frames();

    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = (uint32_t)kernel_directory->tablesPhysical;
    current_directory = kernel_directory;

    /* Map Kernel Heap Area */
    for (uint32_t i = KHEAP_START; i < (KHEAP_START + KHEAP_INITIAL_SIZE); i += PAGE_SIZE)
    {
        create_page(i, kernel_directory);
    }

    /* ALlocate Frames */
    for (uint32_t i = 0; i < (placement_addr + PAGE_SIZE); i += PAGE_SIZE)
    {
        const page_t *page = get_page(i, kernel_directory);
        if (!page)
            page = create_page(i, kernel_directory);
        alloc_frame((page_t *)page, 0, 0);
    }

    /* Allocate Frames for Kernel Heap */
    for (uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
    {
        const page_t *page = get_page(i, kernel_directory);
        if (!page)
            page = create_page(i, kernel_directory);
        alloc_frame((page_t *)page, 0, 0);
    }

    isr_register_interrupt_handler(14, page_fault);
    switch_page_directory(kernel_directory);
    enable_paging((page_directory_t *)&kernel_directory->tablesPhysical);
    init_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_SIZE, 0, 0);
    paging_enabled = true;
}