/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frames.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 14:01:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/13 14:06:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _FRAMES_H
#define _FRAMES_H

#include <memory/paging.h>

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

extern void set_frame(uint32_t addr);
extern void clear_frame(uint32_t addr);
extern uint32_t check_frame(uint32_t addr);
extern uint32_t get_first_frame();
extern void alloc_frame(Page *page, bool is_kernel, bool is_writeable);
extern void free_frame(Page *page);

extern uint32_t *__frames;
extern uint32_t __nframes;
extern uint32_t __placement_address;

#endif /* !_FRAMES_H */