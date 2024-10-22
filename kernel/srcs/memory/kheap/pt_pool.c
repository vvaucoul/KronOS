/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pt_pool.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:35:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 20:45:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mm/pt_pool.h>
#include <stddef.h>
#include <string.h>
#include <system/panic.h>

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
	// Allocate memory for the page table pool with proper alignment
	page_table_pool.base = (uint8_t *)ealloc_aligned(PAGE_TABLE_POOL_SIZE, PAGE_SIZE);
	if (!page_table_pool.base) {
		__PANIC("Failed to initialize page table pool: Allocation failed");
	}

	// Ensure the allocated memory is zeroed out
	if (memset_s(page_table_pool.base, PAGE_TABLE_POOL_SIZE, 0, PAGE_TABLE_POOL_SIZE) != 0) {
		__PANIC("Failed to initialize page table pool: Memory initialization failed");
	}

	// Set the size and reset the offset
	page_table_pool.size = PAGE_TABLE_POOL_SIZE;
	page_table_pool.offset = 0;
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
		return NULL;
	}

	// Ensure the allocation is PAGE_SIZE aligned
	size_t aligned_offset = (page_table_pool.offset + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	if (aligned_offset + sizeof(page_table_t) > page_table_pool.size) {
		// Pool exhausted after alignment adjustment
		return NULL;
	}

	page_table_t *addr = (page_table_t *)(page_table_pool.base + aligned_offset);
	page_table_pool.offset = aligned_offset + sizeof(page_table_t);

	// Initialize the allocated memory
	if (memset_s(addr, sizeof(page_table_t), 0, sizeof(page_table_t)) != 0) {
		__PANIC("Failed to allocate page table: Memory initialization failed");
	}

	return addr;
}