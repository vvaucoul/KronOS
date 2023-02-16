/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:07:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/16 22:16:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <memory/paging.h>
#include <memory/kheap.h>
#include <memory/frames.h>
#include <memory/memory_map.h>

extern uint32_t *kernel_stack;

#endif /* MEMORY_H */