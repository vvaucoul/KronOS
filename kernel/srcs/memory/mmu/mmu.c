/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 19:53:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/mmu.h>	 // Memory Management Unit
#include <mm/mmuf.h> // Memory Management Unit Frames

#include <mm/ealloc.h> // Early Alloc (ealloc, ealloc_aligned)

#include <mm/mm.h> // Memory Management

#include <multiboot/multiboot.h> // Multiboot (getmem)

extern uint32_t kernel_stack;

static page_directory_t *kernel_directory __attribute__((aligned(4096))) = NULL;
static page_directory_t *current_directory __attribute__((aligned(4096))) = NULL;

page_directory_t *mmu_get_current_directory(void) {
	return (current_directory);
}

page_directory_t *mmu_get_kernel_directory(void) {
	return (kernel_directory);
}

void mmu_set_current_directory(page_directory_t *dir) {
	if (dir == NULL) return;
	current_directory = dir;
}

page_t *mmu_get_page(uint32_t address, page_directory_t *dir) {
	if (dir == NULL) return (NULL);

	uint32_t page_idx = address / PAGE_SIZE;
	uint32_t table_idx = page_idx / PAGE_ENTRIES;

	if (dir->tables[table_idx]) {
		if (dir->tables[table_idx]->pages[page_idx % PAGE_ENTRIES].present) {
			return (&dir->tables[table_idx]->pages[page_idx % PAGE_ENTRIES]);
		}
	}

	return (NULL);
}

page_t *mmu_create_page(uint32_t address, page_directory_t *dir) {
	if (dir == NULL) return (NULL);

	uint32_t page_idx = address / PAGE_SIZE;
	uint32_t table_idx = page_idx / PAGE_ENTRIES;

	if (!dir->tables[table_idx]) {
		dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &dir->tablesPhysical[table_idx]);
		memset(dir->tables[table_idx], 0, PAGE_SIZE);
		dir->tablesPhysical[table_idx] |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
	}

	return (&dir->tables[table_idx]->pages[page_idx % PAGE_ENTRIES]);
}

void mmu_destroy_page(uint32_t address, page_directory_t *dir) {
	address /= PAGE_SIZE;
	uint32_t table_idx = address / PAGE_ENTRIES;
	if (dir->tables[table_idx]) {
		dir->tables[table_idx]->pages[address % PAGE_ENTRIES].frame = 0;
	}
}

void mmu_destroy_page_directory(page_directory_t *dir) {
	for (int i = 0; i < PAGE_ENTRIES; i++) {
		if (dir->tables[i]) {
			for (int j = 0; j < PAGE_ENTRIES; j++) {
				free_frame(&dir->tables[i]->pages[j]);
			}
			kfree(dir->tables[i]);
		}
	}
	kfree(dir);
}

void mmu_switch_page_directory(page_directory_t *dir) {
	current_directory = dir;
	switch_page_directory((void *)dir->physicalAddr);
}

page_directory_t *mmu_clone_page_directory(page_directory_t *src) {
	uint32_t phys, offset;

	if (src == NULL) {
		__THROW("Source page directory is NULL!", NULL);
	}

	// Make a new page directory and obtain its physical address
	page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phys);

	if (dir == NULL) {
		__THROW("Failed to allocate memory for new page directory!", NULL);
	} else if (phys == 0) {
		__THROW("Failed to obtain physical address of new page directory!", NULL);
	}

	// Ensure that it is blank
	memset(dir, 0, sizeof(page_directory_t));

	// Get the offset of tablesPhysical from the start of the page_directory_t structure
	offset = (uint32_t)dir->tablesPhysical - (uint32_t)dir;
	dir->physicalAddr = phys + offset;

	if (dir->physicalAddr == 0) {
		__THROW("Failed to obtain physical address of new page directory!", NULL);
	}

	// Go through each page table. If the page table is in the kernel directory, do not make a new copy
	for (int32_t i = 0; i < PAGE_ENTRIES; i++) {
		if (!src->tables[i])
			continue;

		if (kernel_directory->tables[i] == src->tables[i]) {
			// It's in the kernel, so just use the same pointer
			dir->tables[i] = src->tables[i];
			dir->tablesPhysical[i] = src->tablesPhysical[i];
		} else {
			// Make a new page table, which is page aligned
			page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &phys);
			if (table == NULL) {
				__THROW("Failed to allocate memory for new page table!", NULL);
			} else if (phys == 0) {
				__THROW("Failed to obtain physical address of new page table!", NULL);
			}
			memset(table, 0, sizeof(page_table_t));

			// For every entry in the table...
			for (int32_t j = 0; j < PAGE_ENTRIES; j++) {
				if (src->tables[i]->pages[j].frame) {
					// Get a new frame
					allocate_frame(&table->pages[j], 0, 0);
					if (!table->pages[j].frame) {
						__WARND("Failed to allocate frame for page %d!", j);
						continue;
					}

					// Clone the flags from source to destination
					table->pages[j].present = src->tables[i]->pages[j].present;
					table->pages[j].rw = src->tables[i]->pages[j].rw;
					table->pages[j].user = src->tables[i]->pages[j].user;
					table->pages[j].accessed = src->tables[i]->pages[j].accessed;
					table->pages[j].dirty = src->tables[i]->pages[j].dirty;

					copy_page_physical(src->tables[i]->pages[j].frame * PAGE_SIZE, table->pages[j].frame * PAGE_SIZE);
				}
			}
			dir->tables[i] = table;
			dir->tablesPhysical[i] = phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
		}
	}

	// Return the new page directory
	return dir;
}

uint32_t mmu_get_physical_address(uint32_t virtual_address) {
	uint32_t offset = virtual_address & 0xFFF;
	uint32_t table_idx = virtual_address >> 22;
	uint32_t page_idx = (virtual_address >> 12) & 0x3FF;

	page_directory_t *dir = current_directory;
	page_table_t *table = dir->tables[table_idx];
	page_t *page = &table->pages[page_idx];

	return (page->frame * PAGE_SIZE + offset);
}

#include <macros.h>
uint32_t mmu_get_virtual_address(__unused__ uint32_t physical_address) {
#warning "mmu_get_virtual_address not implemented"
	return (0);
}

int mmu_is_paging_enabled() {
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	return (cr0 & 0x80000000) ? 1 : 0;
}

int mmu_init(void) {

	/* Init frames */
	uint32_t mem_size = (multiboot_get_mem_lower() + multiboot_get_mem_upper()) * 1024;
	init_frames(mem_size);

	/* Init kernel directory */
	kernel_directory = (page_directory_t *)ealloc_aligned(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));

	current_directory = kernel_directory;

	/*
	 * Create page tables and allocate frames for the kernel's virtual address space
	 * and the kernel's physical memory.
	 */

	/*
	 * Create page tables for the kernel's virtual address space (0xC0000000 to 0xC0100000).
	 * We don't allocate frames for these pages yet, as we'll do that in the next loop.
	 */
	for (uint32_t i = HEAP_START; i < (HEAP_START + HEAP_INITIAL_SIZE); i += PAGE_SIZE) {
		mmu_create_page(i, kernel_directory);
	}

	/*
	 * Allocate frames for the kernel's physical memory (from 0 to the current
	 * placement address plus one page size). We also create page tables for any
	 * missing pages in this range.
	 *
	 * We must create page tables for the kernel's physical memory used before enabling paging,
	 */
	for (uint32_t i = 0; i < get_placement_addr() + PAGE_SIZE; i += PAGE_SIZE) {
		page_t *page = mmu_get_page(i, kernel_directory);

		if (page == NULL) {
			page = mmu_create_page(i, kernel_directory);
		}
		allocate_frame(page, 0, 0);
	}

	/*
	 * Allocate frames for the kernel's virtual address space (0xC0000000 to 0xC0100000).
	 * We've already created page tables for these pages in the first loop.
	 */
	for (uint32_t i = HEAP_START; i < (HEAP_START + HEAP_INITIAL_SIZE); i += PAGE_SIZE) {
		page_t *page = mmu_get_page(i, kernel_directory);

		if (page == NULL) {
			page = mmu_create_page(i, kernel_directory);
		}
		allocate_frame(page, 0, 0);
	}

	kernel_directory->physicalAddr = (uint32_t)kernel_directory->tablesPhysical;

	isr_register_interrupt_handler(14, mmu_page_fault_handler);
	load_page_directory((void *)kernel_directory->physicalAddr);

	/*
	 * Create the kernel heap
	 * We must create the kernel heap before enabling paging, as the heap
	 * will be allocated in the kernel's virtual address space.
	 */

	enable_paging((void *)kernel_directory->physicalAddr);

	create_heap(HEAP_START, HEAP_START + HEAP_INITIAL_SIZE, HEAP_MAX_SIZE);

	/*
	 * Now, for multi-tasking, we need to allocate a new page directory for the kernel.
	 * We can't use the current page directory, as it's mapped to the kernel's virtual address space.
	 */
	current_directory = mmu_clone_page_directory(kernel_directory);
	mmu_switch_page_directory(current_directory);

	return (0);
}