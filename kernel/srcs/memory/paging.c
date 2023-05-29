/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 18:53:23 by vvaucoul         ###   ########.fr       */
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
    __addr_validator(addr);

    uint32_t page_idx = (uint32_t)addr / PAGE_SIZE;
    uint32_t offset = (uint32_t)addr % PAGE_SIZE;

    page_t *page = get_page(page_idx, kernel_directory);
    if (!page)
        __THROW("Page not found", NULL);
    if (!(page->present))
        __THROW("Page not present", NULL);

    uint32_t physical_addr = (page->frame * PAGE_SIZE) + offset;

    if (physical_addr >= KHEAP_START)
        physical_addr -= KHEAP_START;

    return ((void *)physical_addr);
}

void *get_virtual_address(void *addr)
{
    __addr_validator(addr);

    uint32_t page_idx = (uint32_t)addr / PAGE_SIZE;
    uint32_t offset = (uint32_t)addr % PAGE_SIZE;

    page_t *page = get_page(page_idx, kernel_directory);
    if (!page)
        __THROW("Page not found", NULL);
    if (!(page->present))
        __THROW("Page not present", NULL);

    uint32_t virtual_addr = (page->frame * PAGE_SIZE) + offset;
    return ((void *)(virtual_addr - KERNEL_VIRTUAL_BASE));
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
    {
        if (dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE].present)
            return (&dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE]);
        else
            return (NULL);
    }
    else
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

page_directory_t *create_page_directory()
{
    // Allocate a page-aligned block of memory for the page directory
    page_directory_t *dir = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    if (!dir)
        __THROW("Failed to allocate page directory", NULL);

    // Zero out the page directory
    memset(dir, 0, sizeof(page_directory_t));

    // Set the page directory's physical address
    dir->physicalAddr = (uint32_t)dir - KERNEL_VIRTUAL_BASE;

    // Map the first 4 MB of memory to the page directory
    dir->tablesPhysical[0] = KERNEL_BASE | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;

    return dir;
}

void destroy_page_directory(page_directory_t *dir)
{
    if (dir)
    {
        // Free all page tables in the page directory
        for (int i = 0; i < 1024; ++i)
        {
            if (dir->tables[i])
            {
                page_table_t *table = dir->tables[i];
                for (int j = 0; j < 1024; ++j)
                {
                    if (table->pages[j].frame)
                        free_frame(&table->pages[j]);
                }
                kfree(table);
            }
        }

        // Free the page directory
        kfree(dir);
    }
}

void switch_page_directory(page_directory_t *dir)
{
    if (!paging_enabled)
        __THROW_NO_RETURN(E_PAGING_NOT_ENABLED, NULL);
    if (!dir)
        __THROW_NO_RETURN(E_SWITCH_PAGE_DIRECTORY, NULL);
    current_directory = dir;

    printk("Switching page directory to 0x%x\n", dir->physicalAddr);
    __asm__ volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging
    __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));
}

void init_paging(void)
{
    init_frames();

    // Allocate kernel directory and initialize
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    if (!kernel_directory)
        __PANIC("Failed to allocate memory for kernel directory");
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

    // Enable paging
    enable_paging((page_directory_t *)&kernel_directory->tablesPhysical);


    // Set paging_enabled flag
    paging_enabled = true;

    // Switch to the kernel directory
    switch_page_directory(kernel_directory);

    // Initialize heap memory allocation system
    init_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_SIZE, 0, 0);
}