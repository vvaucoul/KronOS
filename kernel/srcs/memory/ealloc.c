/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ealloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 18:09:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:57:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h>
#include <multiboot/multiboot_mmap.h>
#include <stddef.h>
#include <string.h>

uint32_t placement_addr = (uint32_t)(&__kernel_section_end);

/**
 * Allocates a block of memory of the specified size.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if size is 0.
 */
void *ealloc(uint32_t size) {
	if (size == 0) {
		return (NULL);
	}
	void *addr = (void *)placement_addr;
	placement_addr += size;
	return (addr);
}

/**
 * Allocates a block of memory of the specified size and aligns it to the next page boundary.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if size is 0.
 */
void *ealloc_aligned(uint32_t size) {
	if ((placement_addr & 0xFFFFF000) != 0) {
		placement_addr &= 0xFFFFF000;
		placement_addr += 0x1000;
	}
	void *addr = (void *)placement_addr;
	placement_addr += size;
	return addr;
}

/**
 * Allocates a block of memory of the specified size and aligns it to the next page boundary.
 *
 * @param size The size of the memory block to allocate.
 * @param phys A pointer to a variable that will receive the physical address of the allocated memory block.
 * @return A pointer to the allocated memory block, or NULL if size is 0.
 */
void *ealloc_aligned_physic(uint32_t size, uint32_t *phys) {
	if ((placement_addr & 0xFFFFF000) != 0) {
		placement_addr &= 0xFFFFF000;
		placement_addr += 0x1000;
	}
	void *addr = (void *)placement_addr;
	if (phys) {
		*phys = placement_addr;
	}
	placement_addr += size;
	return addr;
}

/**
 * Allocates a block of memory of the specified size and initializes it with zeros.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if size is 0.
 */
void *ecalloc(uint32_t size) {
	void *addr = ealloc(size);
	if (addr) {
		memset(addr, 0, size);
	}
	return (addr);
}

/**
 * Inserts a block of memory into the specified address.
 *
 * @param ptr A pointer to the destination memory block.
 * @param addr The source address of the memory block to insert.
 * @param size The size of the memory block to insert.
 */
void einsert(void *ptr, uint32_t addr, uint32_t size) {
	memcpy(ptr, (void *)addr, size);
}

/**
 * Returns the current placement address.
 *
 * @return The current placement address.
 */
uint32_t get_placement_addr(void) {
	return (placement_addr);
}

/**
 * Sets the placement address to the specified value.
 *
 * @param addr The new placement address.
 */
void set_placement_addr(uint32_t addr) {
	placement_addr = addr;
}