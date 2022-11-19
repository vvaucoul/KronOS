/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 15:31:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 17:55:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memory.h"

uint32_t *frames = NULL;
uint32_t nframes = 0;

static void __set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void __unset_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static int __check_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static uint32_t __get_first_free_frame(void)
{
    uint32_t j = 0;

    for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] == 0xFFFFFFFF)
            continue;
        for (j = 0; j < (sizeof(uint32_t) * 8); j++)
        {
            if ((frames[i] & 0x1 << j) == 0)
            {
                return i * sizeof(uint32_t) * 8 + j;
            }
        }
    }
    return (-1);
}

void init_frames(void)
{
    nframes = PHYS_MEM_SIZE / PAGE_SIZE;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes));
    if (!frames)
        kernel_panic("init_frames: frames is NULL");
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    printk("Frames : 0x%x | %d\n", frames, nframes);
}

void alloc_frame(t_page *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
        return;
    uint32_t idx = __get_first_free_frame();
    if (idx == (uint32_t)-1)
        kernel_panic("No free frames !");
    __set_frame(idx * PAGE_SIZE);
    page->present = 1;
    page->rw = (is_writeable) ? 1 : 0;
    page->user = (is_kernel) ? 0 : 1;
    page->frame = idx;
}

void free_frame(t_page *page)
{
    uint32_t frame;

    if (!(frame = page->frame))
        return;
    __unset_frame(frame);
    page->frame = 0x0;
}