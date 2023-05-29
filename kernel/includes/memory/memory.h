/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:07:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 17:53:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <memory/paging.h>
#include <memory/kheap.h>
#include <memory/frames.h>
#include <memory/memory_map.h>

#define KERNEL_BASE __kernel_physical_memory_start
#define KERNEL_VIRTUAL_BASE 0xC0000000

static uint32_t *kernel_stack = NULL;

#endif /* MEMORY_H */