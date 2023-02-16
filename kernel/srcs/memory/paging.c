/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 20:57:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/paging.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;
bool paging_enabled = false;

void *get_physical_address(void *addr)
{
    uint32_t page_idx = (uint32_t)addr / PAGE_SIZE;
    uint32_t offset = (uint32_t)addr % PAGE_SIZE;

    page_t *page = get_page(page_idx, kernel_directory);
    if (!page)
        return (NULL);

    uint32_t physical_addr = (page->frame * PAGE_SIZE) + offset;

    if (physical_addr >= KHEAP_START)
        physical_addr -= KHEAP_START;

    return ((void *)physical_addr);
}

void *get_virtual_address(void *addr)
{
    uint32_t page_idx = (uint32_t)addr / PAGE_SIZE;
    uint32_t offset = (uint32_t)addr % PAGE_SIZE;

    page_t *page = get_page(page_idx, kernel_directory);
    if (!page || !(page->present))
        return NULL;

    uint32_t virtual_addr = (page->frame * PAGE_SIZE) + offset;
    return ((void *)virtual_addr);
}

page_t *create_page(uint32_t address, page_directory_t *dir)
{
    uint32_t page_idx = address / PAGE_SIZE;
    uint32_t table_idx = page_idx / 1024;

    if (!dir->tables[table_idx])
    {
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    return (&dir->tables[table_idx]->pages[page_idx % 1024]);
}

page_t *get_page(uint32_t address, page_directory_t *dir)
{
    uint32_t page_idx = address / PAGE_SIZE;
    uint32_t table_idx = page_idx / PAGE_TABLE_SIZE;

    if (dir->tables[table_idx])
        return (&dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE]);
    return (NULL);
}

page_t *create_user_page(uint32_t address, uint32_t end_addr, page_directory_t *dir)
{
    // Determine the table index and page index for the start address
    uint32_t table_idx = address / PAGE_SIZE / 1024;
    uint32_t page_idx = (address / PAGE_SIZE) % 1024;

    // Allocate a new page table if one does not exist for the table index
    if (!dir->tables[table_idx])
    {
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    // Make all pages in the range [address, end_addr) user-accessible
    while (address < end_addr)
    {
        // Determine the current table index and page index
        uint32_t current_table_idx = address / PAGE_SIZE / 1024;
        uint32_t current_page_idx = (address / PAGE_SIZE) % 1024;

        // Allocate a new page table if one does not exist for the current table index
        if (!dir->tables[current_table_idx])
        {
            dir->tables[current_table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[current_table_idx]);
            memset(dir->tables[current_table_idx], 0, PAGE_SIZE);
            dir->tablesPhysical[current_table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        }

        // Set the user-accessible flag for the current page
        dir->tables[current_table_idx]->pages[current_page_idx].user = 1;

        // Move to the next page
        address += PAGE_SIZE;
    }

    // Return a pointer to the first page in the range
    return &dir->tables[table_idx]->pages[page_idx];
}

void destroy_user_page(page_t *page, page_directory_t *dir)
{
    // Determine the table index and page index for the page
    uint32_t table_idx = (uint32_t)page / PAGE_SIZE / 1024;
    uint32_t page_idx = ((uint32_t)page / PAGE_SIZE) % 1024;

    // Clear the user-accessible flag for the page
    dir->tables[table_idx]->pages[page_idx].user = 0;
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    __asm__ volatile("mov %0, %%cr3" ::"r"(dir->physicalAddr));
}

void init_paging(void)
{
    init_frames();

    // Allocate kernel directory and initialize
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = (uint32_t)kernel_directory->tablesPhysical;

    // Set current directory to kernel directory
    current_directory = kernel_directory;

    // Map kernel heap area
    for (uint32_t i = KHEAP_START; i < (KHEAP_START + KHEAP_INITIAL_SIZE); i += PAGE_SIZE)
    {
        create_page(i, kernel_directory);
    }

    // Allocate frames for all memory used before kmalloc is available
    for (uint32_t i = 0; i < (placement_addr + PAGE_SIZE); i += PAGE_SIZE)
    {
        const page_t *page = get_page(i, kernel_directory);
        if (!page)
            page = create_page(i, kernel_directory);
        alloc_frame((page_t *)page, 0, 0);
    }

    // Allocate frames for kernel heap
    for (uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
    {
        const page_t *page = get_page(i, kernel_directory);
        if (!page)
            page = create_page(i, kernel_directory);
        alloc_frame((page_t *)page, 0, 0);
    }

    // Register the page fault handler
    isr_register_interrupt_handler(14, page_fault);

    // Switch to the kernel directory
    switch_page_directory(kernel_directory);

    // Enable paging
    enable_paging((page_directory_t *)&kernel_directory->tablesPhysical);

    // Initialize heap memory allocation system
    init_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_SIZE, 0, 0);

    // Set paging_enabled flag
    paging_enabled = true;
}