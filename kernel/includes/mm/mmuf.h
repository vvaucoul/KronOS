/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmuf.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:39:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 11:56:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MMUF_H
#define MMUF_H

#include <mm/mmu.h>

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

/* Frame functions */
extern void allocate_frame(page_t *page, int is_kernel, int is_writeable);
extern void free_frame(page_t *page);
extern void init_frames(uint64_t mem_size);
extern uint32_t test_frame(uint32_t frame_addr);

extern uint32_t get_frame_count(void);

#endif /* !MMUF_H */