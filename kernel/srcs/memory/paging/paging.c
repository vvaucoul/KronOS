/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/30 14:25:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/paging.h>

#include <system/isr.h>
#include <system/serial.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;
bool paging_enabled = false;

// ! ||--------------------------------------------------------------------------------||
// ! ||                            UTILS - TRANSLATE ADDRESS                           ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Retrieves the physical address corresponding to a given virtual address
 *
 * @param dir The page directory
 * @param addr The virtual address
 * @return The physical address corresponding to the virtual address
 */
void *get_physical_address(page_directory_t *dir, void *addr) {
    if (!addr)
        return NULL;
    if (!paging_enabled)
        return (void *)((uint32_t)addr - KERNEL_VIRTUAL_BASE);

    uint32_t page_dir_idx = PAGEDIR_INDEX(addr);
    uint32_t page_tbl_idx = PAGETBL_INDEX(addr);
    uint32_t offset = PAGEFRAME_INDEX(addr);

    if (!dir->tables[page_dir_idx]) {
        __THROW("Page directory entry not present!", NULL);
    }

    page_table_t *table = dir->tables[page_dir_idx];

    if (!table->pages[page_tbl_idx].present) {
        __THROW("Page table entry not present!", NULL);
    }

    uint32_t physical_addr = (table->pages[page_tbl_idx].frame << 12) + offset;
    return (void *)physical_addr;
}
/**
 * Retrieves the virtual address corresponding to a given physical address
 * within a page directory.
 *
 * @param dir The page directory to search in.
 * @param addr The physical address to find the virtual address for.
 * @return The virtual address corresponding to the given physical address,
 *         or NULL if no mapping exists.
 */
void *get_virtual_address(page_directory_t *dir, void *addr) {
    if (!addr)
        return NULL;
    if (!paging_enabled)
        return (void *)((uint32_t)addr + KERNEL_VIRTUAL_BASE);

    uint32_t page_dir_idx = PAGEDIR_INDEX(addr);
    uint32_t page_tbl_idx = PAGETBL_INDEX(addr);
    uint32_t offset = PAGEFRAME_INDEX(addr);

    if (!dir->tables[page_dir_idx]) {
        __THROW("Page directory entry not present", NULL);
    }

    page_table_t *table = dir->tables[page_dir_idx];

    if (!table->pages[page_tbl_idx].present) {
        __THROW("Page table entry not present", NULL);
    }

    uint32_t physical_addr = (table->pages[page_tbl_idx].frame << 12) + offset;
    return (void *)(physical_addr + KERNEL_VIRTUAL_BASE);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  PAGE MANAGEMENT                               ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Creates a new page entry in the page directory.
 *
 * @param address The physical address of the page.
 * @param dir The page directory in which the page entry will be created.
 * @return A pointer to the newly created page entry.
 */
page_t *create_page(uint32_t address, page_directory_t *dir) {
    uint32_t page_idx = address / PAGE_SIZE;
    uint32_t table_idx = page_idx / PAGE_TABLE_SIZE;

    if (!dir->tables[table_idx]) {
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    return &dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE];
}

/**
 * Retrieves the page entry corresponding to the given address from the specified page directory.
 *
 * @param address The virtual address for which to retrieve the page entry.
 * @param dir     The page directory from which to retrieve the page entry.
 * @return        A pointer to the page entry if found, or NULL if not found.
 */
page_t *get_page(uint32_t address, page_directory_t *dir) {
    uint32_t page_idx = address / PAGE_SIZE;
    uint32_t table_idx = page_idx / PAGE_TABLE_SIZE;

    if (dir && dir->tables[table_idx] && dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE].present) {
        return &dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE];
    }

    return NULL;
}

/**
 * Flushes the Translation Lookaside Buffer (TLB) entry for the given address.
 *
 * @param addr The address for which the TLB entry needs to be flushed.
 */
void flush_tlb_entry(uint32_t addr) {
    __asm__ volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

/**
 * Switches the current page directory to the specified directory.
 *
 * @param dir The page directory to switch to.
 */
void switch_page_directory(page_directory_t *dir) {
    if (!paging_enabled)
        __THROW_NO_RETURN(E_PAGING_NOT_ENABLED);
    if (!dir)
        __THROW_NO_RETURN(E_SWITCH_PAGE_DIRECTORY);

    current_directory = dir;
    __asm__ volatile("mov %0, %%cr3" ::"r"(dir->physicalAddr));

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= CR0_PG_BIT;
    __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));
}

/**
 * Checks if paging is enabled.
 *
 * @return 1 if paging is enabled, 0 otherwise.
 */
int is_paging_enabled(void) {
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    return (cr0 & CR0_PG_BIT) != 0;
}

/**
 * Creates a user page within the specified address range and associates it with the given page directory.
 *
 * @param address   The starting address of the page.
 * @param end_addr  The ending address of the page.
 * @param dir       The page directory to associate the page with.
 * @return          A pointer to the created page.
 */
page_t *create_user_page(uint32_t address, uint32_t end_addr, page_directory_t *dir) {
    uint32_t table_idx = address / PAGE_SIZE / PAGE_TABLE_SIZE;
    uint32_t page_idx = (address / PAGE_SIZE) % PAGE_TABLE_SIZE;

    if (!dir->tables[table_idx]) {
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    while (address < end_addr) {
        uint32_t current_table_idx = address / PAGE_SIZE / PAGE_TABLE_SIZE;
        uint32_t current_page_idx = (address / PAGE_SIZE) % PAGE_TABLE_SIZE;

        if (!dir->tables[current_table_idx]) {
            dir->tables[current_table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[current_table_idx]);
            memset(dir->tables[current_table_idx], 0, PAGE_SIZE);
            dir->tablesPhysical[current_table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        }

        dir->tables[current_table_idx]->pages[current_page_idx].user = 1;
        address += PAGE_SIZE;
    }

    return &dir->tables[table_idx]->pages[page_idx];
}

/**
 * @brief Destroys a user page in the given page directory.
 *
 * This function is responsible for destroying a user page in the specified page directory.
 *
 * @param page The page to be destroyed.
 * @param dir The page directory in which the page resides.
 */
void destroy_user_page(page_t *page, page_directory_t *dir) {
    uint32_t table_idx = (uint32_t)page / PAGE_SIZE / PAGE_TABLE_SIZE;
    uint32_t page_idx = ((uint32_t)page / PAGE_SIZE) % PAGE_TABLE_SIZE;

    dir->tables[table_idx]->pages[page_idx].user = 0;
}

/**
 * Maps a page to a given address with specified flags in the page directory.
 *
 * @param address The physical address to be mapped.
 * @param flags The flags to be set for the mapped page.
 * @param dir The page directory in which the page is to be mapped.
 * @return A pointer to the mapped page.
 */
page_t *map_page(uint32_t address, uint32_t flags, page_directory_t *dir) {
    uint32_t current_table_idx = address / PAGE_SIZE / PAGE_TABLE_SIZE;
    uint32_t current_page_idx = (address / PAGE_SIZE) % PAGE_TABLE_SIZE;

    if (!dir->tables[current_table_idx]) {
        dir->tables[current_table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[current_table_idx]);
        memset(dir->tables[current_table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[current_table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    dir->tables[current_table_idx]->pages[current_page_idx].present = 1;
    dir->tables[current_table_idx]->pages[current_page_idx].rw = (flags & PAGE_WRITE) ? 1 : 0;
    dir->tables[current_table_idx]->pages[current_page_idx].user = (flags & PAGE_USER) ? 1 : 0;

    page_t *page = &dir->tables[current_table_idx]->pages[current_page_idx];
    page->present = 1;
    page->rw = (flags & PAGE_WRITE) ? 1 : 0;
    page->user = (flags & PAGE_USER) ? 1 : 0;

    return page;
}

/**
 * Clones a page table.
 *
 * This function creates a new page table by copying the contents of the source page table.
 * It also updates the physical address of the new page table.
 *
 * @param src The source page table to be cloned.
 * @param physAddr Pointer to the physical address of the new page table.
 * @return The cloned page table.
 */
page_table_t *clone_table(page_table_t *src, uint32_t *physAddr) {
    page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), physAddr);

    if (!table) {
        __THROW("Failed to allocate memory for new page table!", NULL);
    } else if (!*physAddr) {
        __THROW("Failed to obtain physical address of new page table!", NULL);
    }

    memset(table, 0, sizeof(page_table_t));

    for (int32_t i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (src->pages[i].frame) {
            alloc_frame(&table->pages[i], 0, 0);

            if (!table->pages[i].frame) {
                __WARND("Failed to allocate frame for page %d!", i);
                continue;
            }

            table->pages[i].present = src->pages[i].present;
            table->pages[i].rw = src->pages[i].rw;
            table->pages[i].user = src->pages[i].user;
            table->pages[i].accessed = src->pages[i].accessed;
            table->pages[i].dirty = src->pages[i].dirty;

            copy_page_physical(src->pages[i].frame * PAGE_SIZE, table->pages[i].frame * PAGE_SIZE);

            if (!src->pages[i].frame || !table->pages[i].frame) {
                __WARND("Invalid frame for page %d!", i);
                continue;
            }

            if (!IS_PAGE_MAPPED(src->pages[i].frame) || !IS_PAGE_MAPPED(table->pages[i].frame)) {
                map_page(src->pages[i].frame * PAGE_SIZE, PAGE_PRESENT | PAGE_WRITE, kernel_directory);
                continue;
            }

            if (!IS_PAGE_READABLE(src->pages[i].frame) || !IS_PAGE_READABLE(table->pages[i].frame)) {
                src->pages[i].rw = 1;
                continue;
            }

            if (memcmp((void *)(src->pages[i].frame * PAGE_SIZE), (void *)(table->pages[i].frame * PAGE_SIZE), PAGE_SIZE) != 0) {
                __WARND("Physical pages %d are not identical!", i);
            }
        }
    }
    return table;
}

/**
 * Clones a page directory.
 *
 * This function creates a new page directory and copies the contents of the
 * source page directory into it. The source page directory remains unchanged.
 *
 * @param src The source page directory to be cloned.
 * @return A pointer to the newly created page directory.
 */
page_directory_t *clone_page_directory(page_directory_t *src) {
    uint32_t phys, offset;

    if (!src) {
        __THROW("Source page directory is NULL!", NULL);
    }

    page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phys);

    if (!dir) {
        __THROW("Failed to allocate memory for new page directory!", NULL);
    } else if (!phys) {
        __THROW("Failed to obtain physical address of new page directory!", NULL);
    }

    memset(dir, 0, sizeof(page_directory_t));

    offset = (uint32_t)dir->tablesPhysical - (uint32_t)dir;
    dir->physicalAddr = phys + offset;

    if (!dir->physicalAddr) {
        __THROW("Failed to obtain physical address of new page directory!", NULL);
    }

    for (int32_t i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (!src->tables[i])
            continue;

        if (kernel_directory->tables[i] == src->tables[i]) {
            dir->tables[i] = src->tables[i];
            dir->tablesPhysical[i] = src->tablesPhysical[i];
        } else {
            dir->tables[i] = clone_table(src->tables[i], &phys);

            if (!dir->tables[i]) {
                __WARND("Failed to clone table %d!", i);
                continue;
            }

            dir->tablesPhysical[i] = phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        }
    }
    return dir;
}

/**
 * @brief Destroys a page directory.
 *
 * This function is responsible for destroying a page directory and freeing the associated memory.
 *
 * @param dir The page directory to be destroyed.
 */
void destroy_page_directory(page_directory_t *dir) {
    if (dir) {
        for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
            if (dir->tables[i]) {
                if (kernel_directory->tables[i] == dir->tables[i]) {
                    continue;
                }
                page_table_t *table = dir->tables[i];
                for (int j = 0; j < 1024; ++j) {
                    if (table->pages[j].frame) {
                        free_frame(&table->pages[j]);
                    }
                }
                kfree(table);
            }
        }
        kfree(dir);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   INIT PAGING                                  ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Initializes the paging mechanism.
 */
void init_paging(void) {
    init_frames();

    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    if (!kernel_directory)
        __PANIC("Failed to allocate memory for kernel directory");
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = (uint32_t)kernel_directory->tablesPhysical;

    for (uint32_t i = KHEAP_START; i < (KHEAP_START + KHEAP_INITIAL_SIZE); i += PAGE_SIZE) {
        create_page(i, kernel_directory);
    }

    for (uint32_t i = 0; i < (placement_addr + PAGE_SIZE); i += PAGE_SIZE) {
        page_t *page = get_page(i, kernel_directory);
        if (!page) {
            page = create_page(i, kernel_directory);
        }
        alloc_frame(page, 0, 0);
    }

    for (uint32_t i = KHEAP_START; i < (KHEAP_START + KHEAP_INITIAL_SIZE); i += PAGE_SIZE) {
        page_t *page = get_page(i, kernel_directory);
        if (!page)
            page = create_page(i, kernel_directory);
        alloc_frame(page, 0, 0);
    }

    isr_register_interrupt_handler(ISR_PAGE_FAULT, page_fault);

    enable_paging((page_directory_t *)&kernel_directory->tablesPhysical);

    if (is_paging_enabled() == false)
        __PANIC("Paging is not enabled!");
    paging_enabled = true;

    switch_page_directory(kernel_directory);

    init_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_SIZE, 0, 0);

    current_directory = clone_page_directory(kernel_directory);
    switch_page_directory(current_directory);
}