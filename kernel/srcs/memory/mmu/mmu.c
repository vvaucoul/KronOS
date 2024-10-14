/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/02 12:25:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/mmu.h>	 // Memory Management Unit
#include <mm/mmuf.h> // Memory Management Unit Frames

#include <mm/ealloc.h> // Early Alloc (ealloc, ealloc_aligned)

#include <mm/mm.h>				 // Memory Management
#include <multiboot/multiboot.h> // Multiboot (getmem)

extern uint32_t kernel_stack;
static page_directory_t *kernel_directory __attribute__((aligned(4096))) = NULL;
static page_directory_t *current_directory __attribute__((aligned(4096))) = NULL;

/**
 * Retrieves the current page directory.
 *
 * @return A pointer to the current page directory.
 */
page_directory_t *mmu_get_current_directory(void) {
	return (current_directory);
}

/**
 * Retrieves the kernel page directory.
 *
 * @return A pointer to the kernel page directory.
 */
page_directory_t *mmu_get_kernel_directory(void) {
	return (kernel_directory);
}

/**
 * Sets the current page directory for the MMU.
 *
 * @param dir The page directory to set as the current directory.
 */
void mmu_set_current_directory(page_directory_t *dir) {
	if (dir == NULL) return;
	current_directory = dir;
}

/**
 * Retrieves the page corresponding to the given address from the specified page directory.
 *
 * @param address The address for which to retrieve the page.
 * @param dir The page directory from which to retrieve the page.
 * @return A pointer to the page structure corresponding to the given address.
 */
page_t *mmu_get_page(uint32_t address, page_directory_t *dir) {
	if (dir == NULL) return (NULL);

	uint32_t table_idx = address / (PAGE_SIZE * PAGE_ENTRIES);
	uint32_t page_idx = (address / PAGE_SIZE) % PAGE_ENTRIES;
	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[page_idx];
	} else {
		return NULL;
	}
}

/**
 * Creates a new page in the memory management unit (MMU).
 *
 * @param address The address of the page.
 * @param dir The page directory to associate the page with.
 * @return A pointer to the newly created page.
 */
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

/**
 * @brief Destroys a page in the memory management unit (MMU).
 *
 * This function is responsible for destroying a page in the MMU given its address and the page directory.
 *
 * @param address The address of the page to be destroyed.
 * @param dir The page directory containing the page.
 */
void mmu_destroy_page(uint32_t address, page_directory_t *dir) {
	if (dir == NULL) return;

	address /= PAGE_SIZE;
	uint32_t table_idx = address / PAGE_ENTRIES;
	if (dir->tables[table_idx]) {
		dir->tables[table_idx]->pages[address % PAGE_ENTRIES].frame = 0;
		dir->tables[table_idx]->pages[address % PAGE_ENTRIES].present = 0;
	}
}

/**
 * @brief Destroy a page directory.
 *
 * This function is responsible for destroying a page directory.
 *
 * @param dir The page directory to be destroyed.
 */
void mmu_destroy_page_directory(page_directory_t *dir) {
	if (dir == NULL) return;

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

/**
 * @brief Switches the current page directory to the specified directory.
 *
 * This function is responsible for switching the current page directory to the
 * specified directory. It updates the page directory register (CR3) with the
 * physical address of the new directory.
 *
 * @param dir The page directory to switch to.
 */
void mmu_switch_page_directory(page_directory_t *dir) {
	if (dir == NULL) return;

	mmu_set_current_directory(dir);
	switch_page_directory((void *)dir->physicalAddr);
}

extern int mmu_compare_page_directories(page_directory_t *dir1, page_directory_t *dir2);

/**
 * Clones a page directory.
 *
 * @param src The source page directory to clone.
 * @return A pointer to the cloned page directory.
 */
page_directory_t *mmu_clone_page_directory(page_directory_t *src) {
	
	// if (src == NULL) return NULL;

	// 	uint32_t phys;
	// 	page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phys);
	// 	if (dir == NULL || phys == 0) return NULL;

	// 	memset(dir, 0, sizeof(page_directory_t));
	// 	uint32_t offset = (uint32_t)dir->tablesPhysical - (uint32_t)dir;
	// 	dir->physicalAddr = phys + offset;

	// 	if (dir->physicalAddr == 0) return NULL;

	// 	for (int32_t i = 0; i < PAGE_ENTRIES; i++) {
	// 		if (!src->tables[i]) continue;

	// 		if (kernel_directory->tables[i] == src->tables[i]) {
	// 			printk("mmu_clone_page_directory: table[%d] -> table[%d]\n", i, i);
	// 			dir->tables[i] = src->tables[i];
	// 			dir->tablesPhysical[i] = src->tablesPhysical[i];
	// 		} else {
	// 			printk("mmu_clone_page_directory: Cloning table[%d]\n", i);
	// 			uint32_t table_phys;
	// 			page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &table_phys);
	// 			if (table == NULL || table_phys == 0) return NULL;
	// 			memset(table, 0, sizeof(page_table_t));

	// 			for (int32_t j = 0; j < PAGE_ENTRIES; j++) {
	// 				if (src->tables[i]->pages[j].frame) {
	// 					allocate_frame(&table->pages[j], 0, 0);
	// 					if (!table->pages[j].frame) continue;

	// 					table->pages[j].present = src->tables[i]->pages[j].present;
	// 					table->pages[j].rw = src->tables[i]->pages[j].rw;
	// 					table->pages[j].user = src->tables[i]->pages[j].user;
	// 					table->pages[j].accessed = src->tables[i]->pages[j].accessed;
	// 					table->pages[j].dirty = src->tables[i]->pages[j].dirty;
	// 					table->pages[j].nx = src->tables[i]->pages[j].nx;

	// 					copy_page_physical(src->tables[i]->pages[j].frame * PAGE_SIZE, table->pages[j].frame * PAGE_SIZE);
	// 				}
	// 			}
	// 			dir->tables[i] = table;
	// 			dir->tablesPhysical[i] = table_phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
	// 		}
	// 	}

	// 	return dir;

	if (src == NULL) return NULL;

	uint32_t phys;
	page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phys);
	if (dir == NULL || phys == 0) return NULL;

	memset(dir, 0, sizeof(page_directory_t));
	uint32_t offset = (uint32_t)dir->tablesPhysical - (uint32_t)dir;
	dir->physicalAddr = phys + offset;

	if (dir->physicalAddr == 0) return NULL;

	for (int32_t i = 0; i < PAGE_ENTRIES; i++) {
		if (!src->tables[i]) continue;

		// Todo: does not really clone, but if i really clone, it crash
		printk("mmu_clone_page_directory: Cloning table[%d]\n", i);
		uint32_t table_phys;
		page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &table_phys);
		if (table == NULL || table_phys == 0) return NULL;
		memset(table, 0, sizeof(page_table_t));

		for (int32_t j = 0; j < PAGE_ENTRIES; j++) {
			if (src->tables[i]->pages[j].frame) {
				allocate_frame(&table->pages[j], 0, 0);
				if (!table->pages[j].frame) continue;

				table->pages[j].present = src->tables[i]->pages[j].present;
				table->pages[j].rw = src->tables[i]->pages[j].rw;
				table->pages[j].user = src->tables[i]->pages[j].user;
				table->pages[j].accessed = src->tables[i]->pages[j].accessed;
				table->pages[j].dirty = src->tables[i]->pages[j].dirty;
				table->pages[j].nx = src->tables[i]->pages[j].nx;

				copy_page_physical(src->tables[i]->pages[j].frame * PAGE_SIZE, table->pages[j].frame * PAGE_SIZE);
			}
		}
		dir->tables[i] = table;
		dir->tablesPhysical[i] = table_phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
	}
	if ((mmu_compare_page_directories(kernel_directory, dir)) == 0) {
		printk("mmu_clone_page_directory: Directories are not identical\n");
	} else {
		printk("mmu_clone_page_directory: Directories are identical\n");
	}
	kpause();

	return dir;
}

/**
 * @brief Retrieves the physical address corresponding to the given virtual address.
 *
 * @param virtual_address The virtual address for which to retrieve the physical address.
 * @return The physical address corresponding to the given virtual address.
 */
uint32_t mmu_get_physical_address(uint32_t virtual_address) {
	uint32_t offset = virtual_address & 0xFFF;
	uint32_t table_idx = virtual_address >> 22;
	uint32_t page_idx = (virtual_address >> 12) & 0x3FF;

	page_directory_t *dir = current_directory;
	page_table_t *table = dir->tables[table_idx];
	page_t *page = &table->pages[page_idx];

	return (page->frame * PAGE_SIZE + offset);
}

/**
 * @brief Retrieves the virtual address corresponding to the given physical address.
 *
 * @param physical_address The physical address for which to retrieve the virtual address.
 * @return The virtual address corresponding to the given physical address.
 */
uint32_t mmu_get_virtual_address(uint32_t physical_address) {
	/* Iterate through the entire address space */
	for (uint32_t i = 0; i < PAGE_ENTRIES; ++i) {
		if (current_directory->tables[i]) {
			for (uint32_t j = 0; j < PAGE_ENTRIES; ++j) {
				page_t *page = &current_directory->tables[i]->pages[j];
				if (page->present && ((page->frame * PAGE_SIZE) == (physical_address & ~0xFFF))) {
					/* Calculate the virtual address */
					uint32_t virtual_address = (i * PAGE_ENTRIES * PAGE_SIZE) + (j * PAGE_SIZE) + (physical_address & 0xFFF);
					return virtual_address;
				}
			}
		}
	}
	return 0; // Return 0 if the physical address is not found
}

/**
 * @brief Check if paging is enabled.
 *
 * This function checks whether paging is enabled or not.
 *
 * @return 1 if paging is enabled, 0 otherwise.
 */
int mmu_is_paging_enabled() {
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	return (cr0 & 0x80000000) ? 1 : 0;
}

/**
 * @brief Flushes the Translation Lookaside Buffer (TLB).
 *
 * The Translation Lookaside Buffer (TLB) is a cache that stores recently used virtual-to-physical address translations.
 * Flushing the TLB clears all the entries in the cache, forcing the CPU to perform new address translations.
 *
 * @note This function does not take any parameters.
 *
 * @return void
 */
void mmu_flush_tlb() {
	__asm__ __volatile__("mov %cr3, %eax; mov %eax, %cr3");
}

/**
 * @brief Flushes the Translation Lookaside Buffer (TLB) entry for the given address.
 *
 * @param address The address for which the TLB entry needs to be flushed.
 */
void mmu_flush_tlb_entry(uint32_t address) {
	__asm__ __volatile__("invlpg (%0)" : : "r"(address) : "memory");
}

/**
 * Sets the NX (No-Execute) bit for the specified memory address.
 *
 * @param address The memory address to set the NX bit for.
 * @param enable  A flag indicating whether to enable or disable the NX bit.
 */
void mmu_set_nx_bit(uint32_t address, int enable) {
	page_t *page = mmu_get_page(address, current_directory);
	if (page) {
		page->nx = enable ? 1 : 0;
		mmu_flush_tlb_entry(address);
	}
}

/**
 * @brief Protects a memory region with specified permissions.
 *
 * This function is used to protect a memory region starting from the given address
 * with the specified size and permissions. The permissions parameter determines
 * the access rights for the protected region.
 *
 * @param address The starting address of the memory region.
 * @param size The size of the memory region to be protected.
 * @param permissions The permissions to be set for the memory region.
 */
void mmu_protect_region(uint32_t address, uint32_t size, int permissions) {
	for (uint32_t i = address; i < address + size; i += PAGE_SIZE) {
		page_t *page = mmu_get_page(i, current_directory);
		if (page) {
			page->rw = (permissions & 0x2) ? 1 : 0;
			page->user = (permissions & 0x4) ? 1 : 0;
			mmu_flush_tlb_entry(i);
		}
	}
}

/**
 * @brief Initializes the memory management unit (MMU).
 *
 * This function initializes the MMU and performs any necessary setup
 * for memory management. It is typically called during system startup.
 *
 * @return 0 on success, a negative error code on failure.
 */
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
	if (mmu_is_paging_enabled() == false) return (-1);
	create_heap(HEAP_START, HEAP_START + HEAP_INITIAL_SIZE, HEAP_MAX_SIZE);

	/*
	 * Now, for multi-tasking, we need to allocate a new page directory for the kernel.
	 * We can't use the current page directory, as it's mapped to the kernel's virtual address space.
	 */

	page_directory_t *new_kernel_directory = mmu_clone_page_directory(kernel_directory);
	if (new_kernel_directory == NULL) return (-1);

	/*
	 * Switch to the new kernel page directory.
	 * This will map the kernel's physical memory to the new page directory.
	 */
	mmu_switch_page_directory(new_kernel_directory);

	return (0);
}