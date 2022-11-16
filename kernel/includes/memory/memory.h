/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:42:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 12:45:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include <kernel.h>
#include "paging.h"

#define MEMORY_START 0x100000
#define PHYSICAL_MEMORY_SIZE 0x100000
// #define PAGE_SIZE 4096

#define USER_MEMORY_START 0x00000000
#define USER_MEMORY_END 0xBFFFFFFF

#define KERNEL_MEMORY_START 0xC0000000
#define KERNEL_MEMORY_END 0xFFFFFFFF

#define MEMORY_MAX (4 * 1024 * 1024 * 1024) // 4 Go
#define MAX_PAGES (MEMORY_MAX / PAGE_SIZE) // 1 048 576 Pages

#endif