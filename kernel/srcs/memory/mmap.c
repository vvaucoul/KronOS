/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 16:58:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/02 17:40:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/mmap.h>

#include <memory/frames.h>
#include <memory/paging.h>

#include <multitasking/process.h>

extern task_t *current_task;
extern task_t *ready_queue;

void *mmap(void *addr, uint32_t length, int prot, int flags) {
    // Ensure the requested address is page-aligned
    if ((uint32_t)addr % PAGE_SIZE != 0) {
        return (NULL);
    }

    // Calculate the number of pages we need to allocate
    uint32_t pages = length / PAGE_SIZE;
    if (length % PAGE_SIZE != 0) {
        pages++;
    }

    // Create and map each page
    for (uint32_t i = 0; i < pages; i++) {
        // Create a new page in the current directory at the requested address
        page_t *page = create_page((uint32_t)addr + i * PAGE_SIZE, current_directory);
        if (!page) {
            return (NULL);
        }

        // Allocate a frame for this page
        alloc_frame(page, 0, 1);

        // Set the page flags
        page->present = 1;
        if (prot & PROT_WRITE) {
            page->rw = 1;
        }
        if (flags & MAP_USER) {
            page->user = 1;
        }
    }

    // Return the start of the mapped memory
    return addr;
}