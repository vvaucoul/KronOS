/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:39:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/27 17:24:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRAMES_H
#define FRAMES_H

#include <kernel.h>
#include <memory/paging.h>

extern uint32_t n_frames;
extern uint32_t *frames;

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);
void init_frames(void);

#endif /* !FRAMES_H */