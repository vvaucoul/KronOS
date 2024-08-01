/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:07:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 00:18:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <kernel.h>

#include <mm/mmuf.h>
#include <mm/kheap.h>
#include <mm/mmu.h>

#define KERNEL_BASE 0x00100000
#define KERNEL_VIRTUAL_BASE 0xC0000000

extern uint32_t *kernel_stack;
extern uint32_t initial_esp;

#endif /* MEMORY_H */