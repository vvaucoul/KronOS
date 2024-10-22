/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagetable_pool.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:35:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 15:57:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mm/pagetable_pool.h>
#include <stddef.h>
#include <string.h>
#include <system/panic.h>

// Debug
#include <system/serial.h>

typedef struct pagetable_pool {
	uint8_t *base;
	size_t size;
	size_t offset;
} pagetable_pool_t;

static pagetable_pool_t page_table_pool;

/**
 * @brief Initializes the page table pool with proper alignment.
 */
void pagetable_pool_init(void) {
	// Use ealloc_aligned to ensure PAGE_SIZE alignment
	page_table_pool.base = (uint8_t *)ealloc_aligned(PAGE_TABLE_POOL_SIZE, PAGE_SIZE);
	if (!page_table_pool.base) {
		// Handle allocation failure
		printk("pagetable_pool_init: Failed to allocate page table pool.\n");
		qemu_printf("pagetable_pool_init: Failed to allocate page table pool.\n");
		__PANIC("Failed to initialize page table pool");
	}
	page_table_pool.size = PAGE_TABLE_POOL_SIZE;
	page_table_pool.offset = 0;
	memset(page_table_pool.base, 0, page_table_pool.size);
	qemu_printf("pagetable_pool_init: Page table pool initialized at %p\n", (void *)page_table_pool.base);
	qemu_printf("pagetable_pool_init: Page table pool end at %p\n", (void *)(page_table_pool.base + page_table_pool.size));
}

/**
 * @brief Allocates a page table from the pool.
 *
 * @return A pointer to the allocated page table, or NULL if the pool is exhausted.
 */
page_table_t *pagetable_pool_alloc(void) {
	// Ensure allocations are PAGE_SIZE aligned
	if (page_table_pool.offset + sizeof(page_table_t) > page_table_pool.size) {
		// Pool exhausted
		qemu_printf("pagetable_pool_alloc: Page table pool exhausted.\n");
		return NULL;
	}

	page_table_t *addr = (page_table_t *)(page_table_pool.base + page_table_pool.offset);
	page_table_pool.offset += sizeof(page_table_t);

	// Initialize the allocated memory
	memset(addr, 0, sizeof(page_table_t));

	qemu_printf("pagetable_pool_alloc: Allocated page table at 0x%p\n", (void *)addr);
	return addr;
}