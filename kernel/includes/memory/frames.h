/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:39:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 12:18:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRAMES_H
#define FRAMES_H

#include <memory/paging.h>
#include <stdint.h>

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

/* Frame functions */
extern void alloc_frame(page_t *page, int is_kernel, int is_writeable);
extern void free_frame(page_t *page);
extern void init_frames(void);
extern int test_frame(uint32_t frame_addr);

#endif /* !FRAMES_H */