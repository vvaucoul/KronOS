/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 14:01:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/18 16:07:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <asm/asm.h>

uint32_t *__frames = NULL;
uint32_t __nframes = 0;
uint32_t __placement_address = 0;

// Set a bit in the frames bitset
void set_frame(uint32_t addr)
{
    uint32_t frame = addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    __frames[idx] |= (0x1 << off);
}

// Clear a bit in the frames bitset
void clear_frame(uint32_t addr)
{
    uint32_t frame = addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    __frames[idx] &= ~(0x1 << off);
}

// Test if a bit is set.
uint32_t check_frame(uint32_t addr)
{
    uint32_t frame = addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (__frames[idx] & (0x1 << off));
}

// Find the first free frame.
uint32_t get_first_frame()
{
    for (uint32_t i = 0; i < INDEX_FROM_BIT(__nframes); i++)
    {
        if (__frames[i] != 0xFFFFFFFF)
        {
            for (uint32_t j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if (!(__frames[i] & toTest))
                {
                    return i * 32 + j;
                }
            }
        }
    }
    return 0;
}

// Allocate a frame.
void alloc_frame(Page *page, bool is_kernel, bool is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        uint32_t idx = get_first_frame();
        if (idx == (uint32_t)-1)
        {
            __PANIC("No free frames!");
        }
        set_frame(idx * PAGE_SIZE);
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
}

// Free a frame.
void free_frame(Page *page)
{
    uint32_t frame;
    if (!(frame = page->frame))
    {
        return;
    }
    else
    {
        clear_frame(frame);
        page->frame = 0x0;
    }
}
