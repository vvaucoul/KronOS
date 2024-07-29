/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:07:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 14:57:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <kernel.h>

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/paging.h>

#define KERNEL_BASE 0x00100000
#define KERNEL_VIRTUAL_BASE 0xC0000000

extern uint32_t *kernel_stack;
extern uint32_t initial_esp;

#endif /* MEMORY_H */