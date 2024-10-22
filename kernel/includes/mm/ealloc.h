/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ealloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 18:07:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/19 19:15:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EALLOC_H
#define EALLOC_H

#include <stdint.h>

/**
 * @file ealloc.h
 * @brief Memory allocation functions for the KronOS kernel.
 *
 * This header file contains declarations for memory allocation functions
 * used in the KronOS kernel. These functions provide various ways to allocate
 * memory, including aligned and physical memory allocations.
 */

/**
 * @brief Allocates a block of memory of the specified size.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block.
 */
void *ealloc(uint32_t size);

/**
 * @brief Allocates a block of memory of the specified size with the specified alignment.
 *
 * @param size The size of the memory block to allocate.
 * @param align_size The alignment size for the memory block.
 * @return A pointer to the allocated memory block.
 */
void *ealloc_aligned(uint32_t size, uint32_t align_size);

/**
 * @brief Allocates a block of memory of the specified size with the specified alignment and returns its physical address.
 *
 * @param size The size of the memory block to allocate.
 * @param align_size The alignment size for the memory block.
 * @param phys A pointer to a variable that will receive the physical address of the allocated memory block.
 * @return A pointer to the allocated memory block.
 */
void *ealloc_aligned_physic(uint32_t size, uint32_t align_size, uint32_t *phys);

/**
 * @brief Allocates a zero-initialized block of memory of the specified size.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block.
 */
void *ecalloc(uint32_t size);

/**
 * @brief Inserts a memory block into the allocation system at the specified address.
 *
 * @param ptr A pointer to the memory block to insert.
 * @param addr The address at which to insert the memory block.
 * @param size The size of the memory block.
 */
void einsert(void *ptr, uint32_t addr, uint32_t size);

/**
 * @brief Gets the current placement address used for memory allocation.
 *
 * @return The current placement address.
 */
uint32_t get_placement_addr(void);

/**
 * @brief Sets the placement address used for memory allocation.
 *
 * @param addr The new placement address.
 */
void set_placement_addr(uint32_t addr);

#endif /* !EALLOC_H */