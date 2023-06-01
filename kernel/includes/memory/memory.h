/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:07:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 13:14:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <memory/paging.h>
#include <memory/kheap.h>
#include <memory/frames.h>
#include <memory/memory_map.h>

#define KERNEL_BASE 0x00100000
#define KERNEL_VIRTUAL_BASE 0xC0000000

extern uint32_t *kernel_stack;
extern uint32_t initial_esp;

#endif /* MEMORY_H */