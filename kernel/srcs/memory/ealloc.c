/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ealloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 18:09:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/21 15:38:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h>
#include <mm/mm.h>
#include <multiboot/multiboot_mmap.h>
#include <stddef.h>
#include <string.h>

// Initial placement address, typically set to the end of the kernel section
static uint32_t placement_addr = (uint32_t)(&_kernel_end) - KERNEL_VIRTUAL_BASE;

/**
 * @brief Allocates a block of memory of the given size.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *ealloc(uint32_t size) {
	if (size == 0) {
		return NULL;
	}
	uint32_t new_addr = placement_addr + size;

	// Check for overflow
	if (new_addr < placement_addr) {
		return NULL;
	}

	// Conversion de l'adresse physique en adresse virtuelle
	void *addr = (void *)(placement_addr + KERNEL_VIRTUAL_BASE);

	placement_addr = new_addr;
	return addr;
}

/**
 * @brief Allocates a block of memory of the given size with the specified alignment.
 *
 * @param size The size of the memory block to allocate.
 * @param align_size The alignment size.
 * @return A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *ealloc_aligned(uint32_t size, uint32_t align_size) {
	if (size == 0) {
		return NULL;
	}
	if (align_size && (placement_addr & (align_size - 1))) {
		placement_addr = (placement_addr + align_size) & ~(align_size - 1);
	}
	uint32_t new_addr = placement_addr + size;

	// Check for overflow
	if (new_addr < placement_addr) {
		return NULL;
	}

	// Conversion de l'adresse physique en adresse virtuelle
	void *addr = (void *)(placement_addr + KERNEL_VIRTUAL_BASE);

	placement_addr = new_addr;
	return addr;
}

/**
 * @brief Allocates a block of memory of the given size with the specified alignment and returns its physical address.
 *
 * @param size The size of the memory block to allocate.
 * @param align_size The alignment size.
 * @param phys A pointer to store the physical address of the allocated memory block.
 * @return A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *ealloc_aligned_physic(uint32_t size, uint32_t align_size, uint32_t *phys) {
	if (size == 0) {
		if (phys) *phys = 0;
		return NULL;
	}
	if (align_size && (placement_addr & (align_size - 1))) {
		placement_addr = (placement_addr + align_size) & ~(align_size - 1);
	}
	uint32_t new_addr = placement_addr + size;

	// Check for overflow
	if (new_addr < placement_addr) {
		return NULL;
	}

	void *addr = (void *)(placement_addr + KERNEL_VIRTUAL_BASE);
	if (phys) *phys = placement_addr;
	placement_addr = new_addr;
	return addr;
}

/**
 * @brief Allocates a block of memory of the given size and initializes it to zero.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *ecalloc(uint32_t size) {
	if (size == 0) {
		return NULL;
	}
	void *addr = ealloc(size);
	if (addr) {
		memset(addr, 0, size);
	}
	return addr;
}

/**
 * @brief Inserts data from a specified address into a given memory block.
 *
 * @param ptr The pointer to the destination memory block.
 * @param addr The source address of the data to insert.
 * @param size The size of the data to insert.
 */
void einsert(void *ptr, uint32_t addr, uint32_t size) {
	if (!ptr || size == 0) {
		return;
	}
	memcpy(ptr, (void *)addr, size);
}

/**
 * @brief Gets the current placement address.
 *
 * @return The current placement address.
 */
uint32_t get_placement_addr(void) {
	return placement_addr;
}

/**
 * @brief Sets the placement address to a specified value.
 *
 * @param addr The new placement address.
 */
void set_placement_addr(uint32_t addr) {
	placement_addr = addr;
}
