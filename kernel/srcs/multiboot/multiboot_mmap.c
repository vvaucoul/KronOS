/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot_mmap.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 11:33:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/21 15:02:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <multiboot/multiboot_mmap.h>

#include <mm/mm.h>

#include <kernel.h>
#include <system/pit.h>

/**
 * Retrieves the available memory sections from the multiboot information structure.
 *
 * @param sections An array of memory_section_t structures to store the memory sections.
 * @param max_sections The maximum number of memory sections that can be stored in the array.
 * @param mb_info_ptr A pointer to the multiboot information structure.
 * @return The number of memory sections retrieved and stored in the array.
 */
int get_available_memory_sections(memory_section_t *sections, int max_sections, void *mb_info_ptr) {
	multiboot_info_t *mb_info = (multiboot_info_t *)mb_info_ptr;

	if (!(mb_info->flags & MULTIBOOT_FLAG_MMAP)) {
		__WARN("No memory map provided by multiboot\n", 0);
		return 0;
	}

	multiboot_mmap_entry_t *mmap = (multiboot_mmap_entry_t *)((uintptr_t)mb_info->mmap_addr);
	uint32_t mmap_end = mb_info->mmap_addr + mb_info->mmap_length;

	int section_count = 0;

	while ((uintptr_t)mmap < (uintptr_t)mmap_end && section_count < max_sections) {
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) { // Usable memory
			sections[section_count].addr = mmap->addr;
			sections[section_count].len = mmap->len;
			section_count++;
		}
		mmap = (multiboot_mmap_entry_t *)((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
	}

	return section_count;
}

uint32_t get_available_memory(void *mb_info_ptr) {
	multiboot_info_t *mb_info = (multiboot_info_t *)mb_info_ptr;

	if (!(mb_info->flags & MULTIBOOT_FLAG_MMAP)) {
		__WARN("No memory map provided by multiboot\n", 0);
		return 0;
	}

	// Conversion de mmap_addr en adresse virtuelle
	multiboot_mmap_entry_t *mmap = (multiboot_mmap_entry_t *)PHYS_TO_VIRT(mb_info->mmap_addr);
	uint32_t mmap_end = mb_info->mmap_addr + mb_info->mmap_length;

	uint32_t available_memory = 0;

	// Boucle sur chaque entrée dans la mmap
	while ((uintptr_t)mmap < PHYS_TO_VIRT(mmap_end)) {
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) { // Mémoire utilisable
			available_memory += mmap->len;
		}
		mmap = (multiboot_mmap_entry_t *)((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
	}

	return available_memory;
}