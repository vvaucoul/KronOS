/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 20:25:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/paging.h>

#include <system/serial.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;
bool paging_enabled = false;

void *get_physical_address(void *addr) {
    __addr_validator(addr, true);

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

void *get_virtual_address(void *addr) {
    __addr_validator(addr, false);

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

page_t *create_page(uint32_t address, page_directory_t *dir) {
    uint32_t page_idx = address / PAGE_SIZE;
    uint32_t table_idx = page_idx / PAGE_TABLE_SIZE;

    if (!dir->tables[table_idx]) {
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    return (&dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE]);
}

page_t *get_page(uint32_t address, page_directory_t *dir) {
    uint32_t page_idx = address / PAGE_SIZE;
    uint32_t table_idx = page_idx / PAGE_TABLE_SIZE;

    if (dir == NULL)
        return (NULL);

    if (dir->tables[table_idx]) {
        if (dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE].present)
            return (&dir->tables[table_idx]->pages[page_idx % PAGE_TABLE_SIZE]);
    }

    return (NULL);
}

int is_paging_enabled(void) {
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(cr0));
    return (cr0 & CR0_PG_BIT) != 0;
}

int verify_paging_enabled() {
    if (!is_paging_enabled()) {
        __THROW("Paging is not enabled!", 1);
    } else {
        printk("Paging is enabled.\n");
        return (0);
    }
}

int verify_page_table(page_table_t *table, uint32_t table_idx) {
    if (!table) {
        __THROW("Page table is not allocated!", 1);
    } else if (table_idx >= PAGE_TABLE_SIZE) {
        __THROW("Invalid page table index!", 1);
    } else if ((uint32_t)table & 0xFFF) {
        __THROW("Page table is not aligned!", 1);
    } else if (!(table->pages[0].present & PAGE_PRESENT)) {
        __THROW("Page table is not present!", 1);
    } else {
        printk("Page table %d is properly set up.\n", table_idx);
        return (0);
    }
}

int verify_page(page_t *page, uint32_t page_idx, uint32_t table_idx) {
    if (!page) {
        __THROW("Page is not present in the page table!", 1);
    } else if (page->present == 0) {
        __THROW("Page is not present in the page table!", 1);
    } else {
        printk("Page %d in table %d is present at frame %d.\n", page_idx, table_idx, page->frame);
        return (0);
    }
}

void display_page_directory(page_directory_t *dir) {
    printk("Page Directory:\n");

    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        if (dir->tables[i]) {
            printk("Page Table %d\n", i);
            printk(" - Physical Address: 0x%x\n", dir->tablesPhysical[i]);
            printk(" - Virtual Address: 0x%x\n", (uint32_t)dir->tables[i]);
            printk(" - Virtual Address (Masked): 0x%x\n", ((uint32_t)dir->tables[i] & PAGE_MASK));

            for (int j = 0; j < PAGE_TABLE_SIZE; ++j) {
                page_t *page = &(dir->tables[i]->pages[j]);
                if (page->present) {
                    uint32_t physical_addr = (page->frame << 12);
                    printk("   - Page %d: Present, Physical Address: 0x%x\n", j, physical_addr);
                }
            }
        }
    }
}

int verify_page_directory(page_directory_t *dir) {
    // Verify that the page directory is properly aligned
    if (!IS_PAGE_ALIGNED((uint32_t)dir)) {
        __THROW("Page directory not aligned!", 1);
    }

    // Verify that the page directory is present
    if (!(dir->tablesPhysical[0] & PAGE_PRESENT)) {
        __THROW("Page directory not present!", 1);
    }

    // Verify that the page directory is mapped to the correct physical address
    uint32_t physical_dir_addr = ((uint32_t)dir->tables & PAGE_MASK);
    uint32_t physical_dir_addr_from_virt = (uint32_t)(dir);
    if (physical_dir_addr != physical_dir_addr_from_virt) {
        __THROW("Page directory not mapped to correct physical address!", 1);
    }

    // Verify that all page tables are properly aligned and marked as present
    for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (dir->tables[i]) {
            if (!IS_PAGE_ALIGNED((uint32_t)dir->tables[i])) {
                __THROW("Page table %d not aligned!", i, 1);
            }
            if (!(dir->tablesPhysical[i] & PAGE_PRESENT)) {
                __THROW("Page table %d not present!", i, 1);
            }
            uint32_t physical_table_addr = (dir->tablesPhysical[i] & PAGE_MASK);
            uint32_t physical_table_addr_from_virt = ((uint32_t)dir->tables[i] & PAGE_MASK);
            if (physical_table_addr != physical_table_addr_from_virt) {
                __THROW("Page table %d not mapped to correct physical address!", 1, i);
            }
            page_table_t *table = dir->tables[i];
            for (uint32_t j = 0; j < PAGE_TABLE_SIZE; ++j) {
                if (table->pages[j].present) {
                    if (!IS_PAGE_ALIGNED((uint32_t) & (table->pages[j]))) {
                        __THROW("Page %d in table %d not aligned!", 1, j, i);
                    }
                    uint32_t physical_page_addr = (table->pages[j].frame << 12);
                    uint32_t physical_page_addr_from_virt = ((uint32_t) & (table->pages[j]) & PAGE_MASK);
                    if (physical_page_addr != physical_page_addr_from_virt) {
                        __THROW("Page %d in table %d not mapped to correct physical address!", 1, j, i);
                    }
                }
            }
        }
    }
    return (0);
}

page_t *create_user_page(uint32_t address, uint32_t end_addr, page_directory_t *dir) {
    // Determine the table index and page index for the start address
    uint32_t table_idx = address / PAGE_SIZE / PAGE_TABLE_SIZE;
    uint32_t page_idx = (address / PAGE_SIZE) % PAGE_TABLE_SIZE;

    // Allocate a new page table if one does not exist for the table index
    if (!dir->tables[table_idx]) {
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
        memset(dir->tables[table_idx], 0, PAGE_SIZE);
        dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    // Make all pages in the range [address, end_addr) user-accessible
    while (address < end_addr) {
        // Determine the current table index and page index
        uint32_t current_table_idx = address / PAGE_SIZE / PAGE_TABLE_SIZE;
        uint32_t current_page_idx = (address / PAGE_SIZE) % PAGE_TABLE_SIZE;

        // Allocate a new page table if one does not exist for the current table index
        if (!dir->tables[current_table_idx]) {
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

void destroy_user_page(page_t *page, page_directory_t *dir) {
    // Determine the table index and page index for the page
    uint32_t table_idx = (uint32_t)page / PAGE_SIZE / PAGE_TABLE_SIZE;
    uint32_t page_idx = ((uint32_t)page / PAGE_SIZE) % PAGE_TABLE_SIZE;

    // Clear the user-accessible flag for the page
    dir->tables[table_idx]->pages[page_idx].user = 0;
}

page_table_t *clone_table(page_table_t *src, uint32_t *physAddr) {
    /* Make a new page table, which is page aligned */
    page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), physAddr);

    if (table == NULL) {
        __THROW("Failed to allocate memory for new page table!", NULL);
    } else if (*physAddr == 0) {
        __THROW("Failed to obtain physical address of new page table!", NULL);
    }

    /* Ensure that the new table is blank */
    memset(table, 0, sizeof(page_directory_t));
    // memset(table, 0, sizeof(page_table_t));

    /* For every entry in the table... */
    for (int32_t i = 0; i < 1024; i++) {
        /* If the source entry has a frame associated with it... */
        if (src->pages[i].frame) {
            /* Get a new frame */
            alloc_frame(&table->pages[i], 0, 0);

            /* Clone the flags from source to destination */
            if (src->pages[i].present)
                table->pages[i].present = 1;
            if (src->pages[i].rw)
                table->pages[i].rw = 1;
            if (src->pages[i].user)
                table->pages[i].user = 1;
            if (src->pages[i].accessed)
                table->pages[i].accessed = 1;
            if (src->pages[i].dirty)
                table->pages[i].dirty = 1;

            copy_page_physical(src->pages[i].frame * PAGE_SIZE, table->pages[i].frame * PAGE_SIZE);
        }
    }
    return table;
}

page_directory_t *clone_page_directory(page_directory_t *src) {
    uint32_t phys, offset;

    if (src == NULL) {
        __THROW("Source page directory is NULL!", NULL);
    }

    /* Make a new page directory and obtain its physical address */
    page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phys);

    if (dir == NULL) {
        __THROW("Failed to allocate memory for new page directory!", NULL);
    } else if (phys == 0) {
        __THROW("Failed to obtain physical address of new page directory!", NULL);
    }

    /* Ensure that it is blank */
    memset(dir, 0, sizeof(page_directory_t));

    /* Get the offset of tablesPhysical from the start of the page_directory_t structure */
    offset = (uint32_t)dir->tablesPhysical - (uint32_t)dir;

    /* Then the physical address of dir->tablesPhysical is */
    dir->physicalAddr = phys + offset;

    if (dir->physicalAddr == 0) {
        __THROW("Failed to obtain physical address of new page directory!", NULL);
    }

    /* Go through each page table. If the page table is in the kernel directory, do not make a new copy */
    for (int32_t i = 0; i < 1024; i++) {
        if (!src->tables[i])
            continue;

        if (kernel_directory->tables[i] == src->tables[i]) {
            /* It's in the kernel, so just use the same pointer */
            dir->tables[i] = src->tables[i];
            dir->tablesPhysical[i] = src->tablesPhysical[i];
        } else {
            /* Copy the table */
            /* uint32_t phys; */
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->tablesPhysical[i] = phys | 0x07;
        }
    }
    return dir;
}

page_table_t kernel_page_tables[1024] __attribute__((aligned(4096)));

page_directory_t *create_page_directory() {
    __PANIC("create_page_directory() is not implemented yet");
    return NULL;
}

void destroy_page_directory(page_directory_t *dir) {
    if (dir) {
        // Free all page tables in the page directory
        for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
            if (dir->tables[i]) {
                page_table_t *table = dir->tables[i];
                for (int j = 0; j < 1024; ++j) {
                    if (table->pages[j].frame) {
                        free_frame(&table->pages[j]);
                    }
                }
                kfree(table);
            }
        }

        // Free the page directory
        kfree(dir);
    }
}

void flush_tlb_entry(uint32_t addr) {
    __asm__ volatile("invlpg (%0)" ::"r"(addr)
                     : "memory");
}

void switch_page_directory(page_directory_t *dir) {
    if (!paging_enabled)
        __THROW_NO_RETURN(E_PAGING_NOT_ENABLED);
    if (!dir)
        __THROW_NO_RETURN(E_SWITCH_PAGE_DIRECTORY);

    current_directory = dir;

    // Set the page directory's physical address
    __asm__ volatile("mov %0, %%cr3" ::"r"(dir->physicalAddr));

    // Read CR0 register and set the paging bit
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(cr0));
    // Enable paging
    cr0 |= 0x80000000;

    // Write back to CR0
    __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));
}

void init_paging(void) {
    init_frames();

    // Allocate kernel directory and initialize
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    if (!kernel_directory)
        __PANIC("Failed to allocate memory for kernel directory");
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = (uint32_t)kernel_directory->tablesPhysical;

    // Map kernel heap area
    for (uint32_t i = KHEAP_START; i < (KHEAP_START + KHEAP_INITIAL_SIZE); i += PAGE_SIZE) {
        create_page(i, kernel_directory);
    }

    // Allocate frames for all memory used before kmalloc is available
    for (uint32_t i = 0; i < (placement_addr + PAGE_SIZE); i += PAGE_SIZE) {
        const page_t *page = get_page(i, kernel_directory);
        if (!page) {
            page = create_page(i, kernel_directory);
        }
        alloc_frame((page_t *)page, 0, 0);
    }

    // Allocate frames for kernel heap
    for (uint32_t i = KHEAP_START; i < (KHEAP_START + KHEAP_INITIAL_SIZE); i += PAGE_SIZE) {
        const page_t *page = get_page(i, kernel_directory);
        if (!page)
            page = create_page(i, kernel_directory);
        alloc_frame((page_t *)page, 0, 0);
    }

    isr_register_interrupt_handler(14, page_fault);

    // Enable paging
    enable_paging((page_directory_t *)&kernel_directory->tablesPhysical);

    // Set paging_enabled flag
    if (is_paging_enabled() == false)
        __PANIC("Paging is not enabled!");
    paging_enabled = true;

    // Switch to the kernel directory
    switch_page_directory(kernel_directory);

    // Initialize heap memory allocation system
    init_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_SIZE, 0, 0);

    current_directory = clone_page_directory(kernel_directory);
    switch_page_directory(current_directory);
}