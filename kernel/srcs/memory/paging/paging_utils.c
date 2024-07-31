/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 17:07:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 11:56:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>

/**
 * Verifies the integrity of a page table entry.
 *
 * This function checks if the given page table entry is valid and properly initialized.
 *
 * @param table A pointer to the page table structure.
 * @param table_idx The index of the page table entry to verify.
 * @return 0 if the page table entry is valid, -1 otherwise.
 */
int verify_page_table(page_table_t *table, uint32_t table_idx) {
    if (!table) {
        __THROW("Page table is not allocated!", 1);
    }
    if (table_idx >= PAGE_TABLE_SIZE) {
        __THROW("Invalid page table index!", 1);
    }
    if ((uint32_t)table & 0xFFF) {
        __THROW("Page table is not aligned!", 1);
    }
    if (!(table->pages[0].present & PAGE_PRESENT)) {
        __THROW("Page table is not present!", 1);
    }
    return 0;
}

/**
 * Verifies the integrity of a page in the paging system.
 *
 * @param page      Pointer to the page to be verified.
 * @param page_idx  Index of the page in the paging system.
 * @param table_idx Index of the page table containing the page.
 *
 * @return          Returns 0 if the page is valid, otherwise returns an error code.
 */
int verify_page(page_t *page, uint32_t page_idx, uint32_t table_idx) {
    if (!page) {
        __THROW("Page is not present in the page table!", 1);
    }
    if (page->present == 0) {
        __THROW("Page is not present in the page table!", 1);
    }
    printk("Page %d in table %d is present at frame %d.\n", page_idx, table_idx, page->frame);
    return 0;
}

/**
 * Displays the contents of a page directory.
 *
 * @param dir The page directory to be displayed.
 */
void display_page_directory(page_directory_t *dir) {
    printk("Page Directory:\n");

    for (uint32_t i = 0; i < PAGE_TABLE_SIZE; ++i) {
        if (dir->tables[i]) {
            printk("Page Table %d\n", i);
            printk(" - Physical Address: 0x%x\n", dir->tablesPhysical[i]);
            printk(" - Virtual Address: 0x%x\n", (uint32_t)dir->tables[i]);
            printk(" - Virtual Address (Masked): 0x%x\n", ((uint32_t)dir->tables[i] & PAGE_MASK));

            for (uint32_t j = 0; j < PAGE_TABLE_SIZE; ++j) {
                page_t *page = &(dir->tables[i]->pages[j]);
                if (page->present) {
                    uint32_t physical_addr = (page->frame << 12);
                    printk("   - Page %d: Present, Physical Address: 0x%x\n", j, physical_addr);
                }
            }
        }
    }
}

/**
 * Verifies the integrity of a page directory.
 *
 * @param dir The page directory to be verified.
 * @return 0 if the page directory is valid, -1 otherwise.
 */
int verify_page_directory(page_directory_t *dir) {
    if (!IS_PAGE_ALIGNED((uint32_t)dir)) {
        __THROW("Page directory not aligned!", 1);
    }

    if (!(dir->tablesPhysical[0] & PAGE_PRESENT)) {
        __THROW("Page directory not present!", 1);
    }

    uint32_t physical_dir_addr = ((uint32_t)dir->tables & PAGE_MASK);
    uint32_t physical_dir_addr_from_virt = (uint32_t)(dir);

    if (physical_dir_addr != physical_dir_addr_from_virt) {
        printk("Physical Address: 0x%x\n", physical_dir_addr);
        printk("Physical Address (Masked): 0x%x\n", physical_dir_addr_from_virt);
        __THROW("Page directory not mapped to correct physical address!", 1);
    }

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
                    if (!IS_FRAME_ALIGNED(table->pages[j].frame)) {
                        __THROW("Page %d in table %d not aligned!", 1, j, i);
                    }
                    verify_page_table(table, i);
                    uint32_t physical_page_addr = (table->pages[j].frame << 12);
                    uint32_t physical_page_addr_from_virt = (uint32_t)VIRTUAL_TO_PHYSICAL((void *)((uint32_t)(table->pages[j].frame << 12)));

                    if (physical_page_addr != physical_page_addr_from_virt) {
                        printk("Physical Address: 0x%x\n", physical_page_addr);
                        printk("Physical Address from virt: 0x%x\n", physical_page_addr_from_virt);
                        __THROW("Page %d in table %d not mapped to correct physical address!", 1, j, i);
                    }
                }
            }
        }
    }
    return 0;
}