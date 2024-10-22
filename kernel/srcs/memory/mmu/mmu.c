/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:34:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 16:58:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/mmu.h>	 // Memory Management Unit
#include <mm/mmuf.h> // Memory Management Unit Frames
#include <mm/pagetable_pool.h>

#include <mm/ealloc.h> // Early Alloc (ealloc, ealloc_aligned)

#include <mm/mm.h>				 // Memory Management
#include <multiboot/multiboot.h> // Multiboot (getmem)

#include <system/serial.h>

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
	if (dir == NULL) return NULL;

	uint32_t table_idx = address / (PAGE_SIZE * PAGE_ENTRIES); // address / 0x400000
	uint32_t page_idx = (address / PAGE_SIZE) % PAGE_ENTRIES;  // (address / 0x1000) % 1024
	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[page_idx];
	} else {
		return NULL;
	}
}

/**
 * @brief Creates a page and maps it in the page directory.
 *
 * @param address The virtual address to map.
 * @param dir The page directory.
 * @param is_kernel Flag indicating if the page is for kernel space.
 * @return A pointer to the created page, or NULL on failure.
 */
page_t *mmu_create_page(uint32_t address, page_directory_t *dir, int is_kernel) {
	if (dir == NULL) return NULL;

	uint32_t page_idx = address / PAGE_SIZE;
	uint32_t table_idx = page_idx / PAGE_ENTRIES;

	if (!dir->tables[table_idx]) {
		// Allocate a page table from the pool
		page_table_t *new_table = pagetable_pool_alloc();
		if (!new_table) {
			qemu_printf("mmu_create_page: Failed to allocate page table from pool.\n");
			return NULL;
		}

		// Zero out the new page table
		memset(new_table, 0, sizeof(page_table_t));

		dir->tables[table_idx] = new_table;

		// Retrieve the physical address of the allocated page table
		uint32_t table_phys = mmu_get_physical_address((void *)new_table);
		if (table_phys == 0) {
			qemu_printf("mmu_create_page: Invalid physical address for page table.\n");
			return NULL;
		}

		dir->tablesPhysical[table_idx] = table_phys | PAGE_PRESENT | PAGE_WRITE;
		if (!is_kernel) {
			dir->tablesPhysical[table_idx] |= PAGE_USER;
		}

		qemu_printf("mmu_create_page: Created new page table at 0x%p (PA=0x%x) for table index %d\n", (void *)new_table, table_phys, table_idx);
	}

	return &dir->tables[table_idx]->pages[page_idx % PAGE_ENTRIES];
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

	// Debugging statement to verify the physical address before switching
	qemu_printf("mmu_switch_page_directory: Switching to directory at physical address 0x%x\n", dir->physicalAddr);

	mmu_set_current_directory(dir);
	switch_page_directory((void *)dir->physicalAddr);
	mmu_flush_tlb();
}

extern int mmu_compare_page_directories(page_directory_t *dir1, page_directory_t *dir2);

/**
 * Clones a page directory.
 *
 * @param src The source page directory to clone.
 * @return A pointer to the cloned page directory.
 */
page_directory_t *mmu_clone_page_directory(page_directory_t *src) {
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

		printk("mmu_clone_page_directory: Cloning table[%d]\n", i);
		uint32_t table_phys;
		page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &table_phys);
		if (table == NULL || table_phys == 0) return NULL;
		memset(table, 0, sizeof(page_table_t));

		for (int32_t j = 0; j < PAGE_ENTRIES; j++) {
			if (src->tables[i]->pages[j].frame) {
				allocate_frame(&table->pages[j], 1, 1); // is_kernel=1, is_writeable=1
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
		dir->tablesPhysical[i] = table_phys | PAGE_PRESENT | PAGE_WRITE; // PAGE_USER n'est pas ajouté pour les pages du kernel
	}

	if (mmu_compare_page_directories(kernel_directory, dir) == 0) {
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
 * @param virt_addr The virtual address.
 * @return The physical address.
 */
uint32_t mmu_get_physical_address(void *virt_addr) {
	uintptr_t va = (uintptr_t)virt_addr;
	if (va < KERNEL_VIRTUAL_BASE) {
		// Identity mapping for lower addresses
		return va;
	}
	// Higher-half mapping: VA = PA + KERNEL_VIRTUAL_BASE
	return va - KERNEL_VIRTUAL_BASE;
}

/**
 * @brief Retrieves the virtual address corresponding to the given physical address.
 *
 * @param physical_address The physical address for which to retrieve the virtual address.
 * @return The virtual address corresponding to the given physical address.
 */
uint32_t mmu_get_virtual_address(uint32_t physical_address) {
	return physical_address + KERNEL_VIRTUAL_BASE;
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

uint8_t mmu_is_protected_mode(void) {
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	return (cr0 & 0x1);
}

void validate_mappings(uint32_t start_addr, uint32_t end_addr) {
	int valid = 1;
	for (uint32_t i = start_addr; i < end_addr; i += PAGE_SIZE) {
		uint32_t physical;
		uint32_t translated_virtual;

		if (i < KERNEL_VIRTUAL_BASE) {
			// Identity mapping (if needed)
			physical = i;
			translated_virtual = physical;
		} else {
			// Higher half mapping
			physical = mmu_get_physical_address((void *)i);			// PA = VA - KERNEL_VIRTUAL_BASE
			translated_virtual = mmu_get_virtual_address(physical); // VA = PA + KERNEL_VIRTUAL_BASE
		}

		if (translated_virtual != i) {
			qemu_printf("Mapping Validation Failed for VA=0x%x: PA=0x%x, Expected VA=0x%x\n", i, physical, translated_virtual);
			valid = 0;
		}
	}
	if (valid) {
		printk("All mappings from 0x%x to 0x%x are valid.\n", start_addr, end_addr);
		qemu_printf("All mappings from 0x%x to 0x%x are valid.\n", start_addr, end_addr);
	} else {
		__PANIC("Some mappings are invalid");
	}
}

void setup_higher_half_mapping(page_directory_t *dir) {
	// Allocate a new page table for the higher-half mapping
	page_table_t *higher_half_pt = pagetable_pool_alloc();
	if (!higher_half_pt) {
		qemu_printf("Failed to allocate higher-half page table.\n");
		__PANIC("Paging setup failed");
	}

	// Zero out the new page table
	memset(higher_half_pt, 0, sizeof(page_table_t));

	// Map physical addresses 0x0 - 0x3FFFFF to virtual addresses 0xC0000000 - 0xC03FFFFF
	for (uint32_t i = 0; i < 1024; i++) {
		higher_half_pt->pages[i].frame = i; // PA = i * PAGE_SIZE
		higher_half_pt->pages[i].present = 1;
		higher_half_pt->pages[i].rw = 1;
		higher_half_pt->pages[i].user = 0; // Kernel mode
	}

	// Assign the page table to the page directory
	dir->tables[KERNEL_PAGE_DIR_INDEX] = higher_half_pt;

	// Calculate the physical address of the page table
	uint32_t higher_half_pt_phys = mmu_get_physical_address((void *)higher_half_pt);

	// Set the physical address in tablesPhysical with present and write flags
	dir->tablesPhysical[KERNEL_PAGE_DIR_INDEX] = higher_half_pt_phys | PAGE_PRESENT | PAGE_WRITE;

	qemu_printf("Higher-half page table allocated at VA=0x%p, PA=0x%x\n", (void *)higher_half_pt, higher_half_pt_phys);
}

/**
 * @brief Initializes the memory management unit (MMU).
 *
 * Cette fonction initialise le MMU et configure les mappings pour le higher half kernel.
 *
 * @return 0 en cas de succès, un code d'erreur négatif en cas d'échec.
 */
int mmu_init(void) {
	if (mmu_is_paging_enabled() == 0) {
		__PANIC("mmu_init: Not in protected mode");
	}

	/* Init frames */
	uint32_t mem_size = (multiboot_get_mem_lower() + multiboot_get_mem_upper()) * 1024;
	init_frames(mem_size);

	/* Init kernel directory */
	kernel_directory = (page_directory_t *)ealloc_aligned(sizeof(page_directory_t), PAGE_SIZE);
	memset(kernel_directory, 0, sizeof(page_directory_t));

	current_directory = kernel_directory;

	/* Initialize the page table pool before creating heap and mapping pages */
	pagetable_pool_init();

	/* Setup higher-half mapping */
	setup_higher_half_mapping(kernel_directory);

	/*
	 * Create page tables for the heap's virtual address space (HEAP_START to HEAP_START + HEAP_INITIAL_SIZE).
	 */
	for (uint32_t i = HEAP_START; i < (HEAP_START + HEAP_INITIAL_SIZE); i += PAGE_SIZE) {
		mmu_create_page(i, kernel_directory, 1);
	}
	qemu_printf("Create page from 0x%x to 0x%x\n", HEAP_START, HEAP_START + HEAP_INITIAL_SIZE);
	printk("Validate mappings [1]\n");
	qemu_printf("Validate mappings [1]\n");
	validate_mappings(HEAP_START, HEAP_START + HEAP_INITIAL_SIZE);

	/*
	 * Allocate frames for the kernel's physical memory (from 0 to the current placement address plus one page size).
	 */
	for (uint32_t i = 0; i < get_placement_addr() + PAGE_SIZE; i += PAGE_SIZE) {
		page_t *page = mmu_get_page(i, kernel_directory);

		if (page == NULL) {
			page = mmu_create_page(i, kernel_directory, 1);
		}
		allocate_frame(page, 1, 1); // is_kernel=1, is_writeable=1
	}
	qemu_printf("Allocate frame from 0x%x to 0x%x\n", 0, get_placement_addr() + PAGE_SIZE);
	printk("Validate mappings [2]\n");
	qemu_printf("Validate mappings [2]\n");
	validate_mappings(0x0, get_placement_addr() + PAGE_SIZE); // Corrected validation

	/*
	 * Allocate frames for the heap's virtual address space (HEAP_START to HEAP_START + HEAP_INITIAL_SIZE).
	 */
	for (uint32_t i = HEAP_START; i < (HEAP_START + HEAP_INITIAL_SIZE); i += PAGE_SIZE) {
		page_t *page = mmu_get_page(i, kernel_directory);

		if (page == NULL) {
			page = mmu_create_page(i, kernel_directory, 1);
		}
		allocate_frame(page, 1, 1); // is_kernel=1, is_writeable=1
	}
	qemu_printf("Allocate frame from 0x%x to 0x%x\n", HEAP_START, HEAP_START + HEAP_INITIAL_SIZE);
	printk("Validate mappings [3]\n");
	qemu_printf("Validate mappings [3]\n");
	validate_mappings(HEAP_START, HEAP_START + HEAP_INITIAL_SIZE);

	/* Set the physical address of the kernel directory */
	kernel_directory->physicalAddr = (uint32_t)kernel_directory->tablesPhysical - KERNEL_VIRTUAL_BASE;

	/* Register the page fault handler */
	isr_register_interrupt_handler(14, mmu_page_fault_handler);

	mmu_switch_page_directory(kernel_directory);
	printk("MMU initialized\n");

	// check if i need to change directory
	// load_page_directory(kernel_directory);

	/* Initialize the heap */
	if (mmu_is_paging_enabled() == false) return (-1);
	initialize_heap(mmu_get_kernel_directory());
	printk("Heap created\n");
	qemu_printf("Heap created\n");

	// Test allocations
	uint32_t size = 0, index = 0;

	while (1) {
		char *foo = kmalloc(0x100);
		if (foo == NULL) {
			__PANIC("Failed to allocate memory");
		}
		size += 0x100;

		strcpy(foo, "Hello, kernel heap!");

		printk("[%ld] Allocated %ld Ko (%ld Mo) 0x%x\n", index, size / 1024, size / (1024 * 1024), foo);
		qemu_printf("[%ld] Allocated %ld Ko (%ld Mo) 0x%x\n", index, size / 1024, size / (1024 * 1024), foo);

		++index;
		// kmsleep(250);
	}

	kpause();

	/*
	 * Maintenant, pour le multi-tasking, nous devons allouer un nouveau page directory pour le kernel.
	 * Nous ne pouvons pas utiliser le current page directory, car il est mappé dans l'espace d'adresses virtuelles du kernel.
	 */

	page_directory_t *new_kernel_directory = mmu_clone_page_directory(kernel_directory);
	if (new_kernel_directory == NULL) return (-1);

	/*
	 * Changer vers le nouveau page directory du kernel.
	 * Cela va mapper la mémoire physique du kernel au nouveau page directory.
	 */
	mmu_switch_page_directory(new_kernel_directory);

	return (0);
}
