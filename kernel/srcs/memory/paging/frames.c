/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:39:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 12:24:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <system/panic.h>

#include <multiboot/multiboot.h>

uint32_t n_frames;
uint32_t *frames;

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
void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
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
    if (!page->frame) {
        return;
    } else {
        clear_frame(page->frame * PAGE_SIZE);
        page->frame = 0x0;
    }
}

/**
 * Initializes the frames for memory paging.
 * This function is responsible for initializing the frames used for memory paging.
 * It performs any necessary setup or initialization tasks.
 */
void init_frames(void) {
    // n_frames = kernel_memory_map.total.total_memory_length * 1024 / PAGE_SIZE;
    n_frames = multiboot_get_mem_upper() * 1024 / PAGE_SIZE;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(n_frames) * sizeof(uint32_t));

    if (!frames) {
        __PANIC("Failed to allocate frames");
    }

    memset_s(frames, INDEX_FROM_BIT(n_frames) * sizeof(uint32_t), 0, INDEX_FROM_BIT(n_frames) * sizeof(uint32_t));
}