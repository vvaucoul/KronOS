/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu_frames.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:39:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 11:56:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/mm.h>
#include <mm/mmuf.h>

#include <mm/ealloc.h>
#include <system/panic.h>

#include <multiboot/multiboot.h>

static uint32_t n_frames;
static uint32_t *frames;

/**
 * Sets the frame at the specified address.
 *
 * @param frame_addr The address of the frame to be set.
 */
static void set_frame(uint32_t frame_addr) {
	uint32_t frame = frame_addr / PAGE_SIZE;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

/**
 * Clears a frame in memory.
 *
 * @param frame_addr The address of the frame to be cleared.
 */
static void clear_frame(uint32_t frame_addr) {
	uint32_t frame = frame_addr / PAGE_SIZE;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

/**
 * @brief Test a frame for a given frame address.
 *
 * This function tests a frame for a given frame address.
 *
 * @param frame_addr The address of the frame to be tested.
 * @return An integer value indicating the result of the test.
 */
uint32_t test_frame(uint32_t frame_addr) {
	uint32_t frame = frame_addr / PAGE_SIZE;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

/**
 * Returns the index of the first available frame in the memory.
 *
 * @return The index of the first available frame.
 */
static uint32_t first_frame(void) {
	for (uint32_t i = 0; i < INDEX_FROM_BIT(n_frames); i++) {
		if (frames[i] != 0xFFFFFFFF) {
			for (uint32_t j = 0; j < 32; j++) {
				uint32_t toTest = 0x1 << j;
				if (!(frames[i] & toTest)) {
					return i * 4 * 8 + j;
				}
			}
		}
	}
	return (uint32_t)-1;
}

/**
 * Allocates a frame for a given page.
 *
 * @param page The page to allocate a frame for.
 * @param is_kernel Flag indicating if the frame is for kernel space.
 * @param is_writeable Flag indicating if the frame is writeable.
 */
void allocate_frame(page_t *page, int is_kernel, int is_writeable) {
	if (page->frame != 0) {
		return;
	} else {
		uint32_t idx = first_frame();
		if (idx == (uint32_t)-1) {
			__PANIC("No free frames!");
		}
		set_frame(idx * PAGE_SIZE);
		page->present = 1;
		page->rw = (is_writeable) ? 1 : 0;
		page->user = (is_kernel) ? 0 : 1;
		page->frame = idx;
	}
}

/**
 * Frees a frame of memory.
 *
 * This function is responsible for freeing a frame of memory that was previously allocated
 * for a page. It takes a pointer to the page structure as a parameter and releases the
 * corresponding frame.
 *
 * @param page A pointer to the page structure.
 */
void free_frame(page_t *page) {
	uint32_t frame;

	if (!(frame = page->frame)) {
		return;
	} else {
		clear_frame(frame * PAGE_SIZE);
		page->frame = 0x0;
	}
}

/**
 * @brief Retrieves the count of frames.
 *
 * This function returns the count of frames in the system.
 *
 * @return The count of frames.
 */
uint32_t get_frame_count(void) {
	return n_frames;
}

/**
 * Initializes the frames for memory paging.
 * This function is responsible for initializing the frames used for memory paging.
 * It performs any necessary setup or initialization tasks.
 */
void init_frames(uint64_t mem_size) {
	n_frames = mem_size / PAGE_SIZE;

	frames = (uint32_t *)ealloc_aligned(INDEX_FROM_BIT(n_frames) * sizeof(uint32_t));
	memset(frames, 0, INDEX_FROM_BIT(n_frames) * sizeof(uint32_t));

	printk("\t   - Total frames: " _GREEN);
	printk("%ld" _END, n_frames);
	printk(" for "_GREEN
		   "(%ld KB)"_END
		   " of memory\n",
		   mem_size / 1024);

	uint32_t frames_per_page = PAGE_SIZE * 8;

	printk("\t   - Frames per page: " _GREEN);
	printk("%ld" _END, frames_per_page);
	printk("\n");
}