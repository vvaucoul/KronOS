/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:39:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>

#include <system/panic.h>

uint32_t n_frames = 0;
uint32_t *frames = NULL;

static void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static uint32_t first_frame(void) {
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(n_frames); i++) {
        if (frames[i] != 0xFFFFFFFF) {
            for (j = 0; j < 32; j++) {
                uint32_t toTest = 0x1 << j;
                if (!(frames[i] & toTest)) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }
    return (-1);
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if (page->frame != 0) {
        return;
    } else {
        uint32_t idx = first_frame();
        if (idx == 0xFFFFFFFF) {
            __PANIC("No free frames!");
        }
        set_frame(idx * PAGE_SIZE);
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
}

void free_frame(page_t *page) {
    uint32_t frame;
    if (!(frame = page->frame)) {
        return;
    } else {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void init_frames() {
    // n_frames = 0x1000000 / PAGE_SIZE;
    n_frames = kernel_memory_map.total.total_memory_length * 1024 / PAGE_SIZE;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(n_frames));

    if (!frames) {
        __PANIC("Failed to allocate frames");
    }

    memset(frames, 0, INDEX_FROM_BIT(n_frames));
}
