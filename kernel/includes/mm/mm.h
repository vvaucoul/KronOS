/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 16:41:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 20:37:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MM_H
#define MM_H

#define PAGE_SIZE 0x1000 // 4KB

#define KERNEL_SPACE_START 0xC0000000			  // 3GB
#define KERNEL_SPACE_END (0xFFFFF000 - 0x1000000) // ~4GB, slightly less to stay within 32-bit limit - KERNEL_SPACE_INITIAL_SIZE (16MB)s
#define KERNEL_SPACE_SIZE 0x3FFF0000			  // ~1GB

#define KERNEL_VIRTUAL_BASE 0xC0000000 // Exemple d'adresse virtuelle de base du noyau
#define PHYS_TO_VIRT(addr) ((void *)((uintptr_t)(addr) + KERNEL_VIRTUAL_BASE))

#define KERNEL_PAGE_DIR_INDEX 768 // (0x1000 * 1024) // 0x30000000 / 0x1000 / 1024

#define ALIGN_UP(addr, align) (((uintptr_t)(addr) + ((align) - 1)) & ~((uintptr_t)((align) - 1)))
#define ALIGN_DOWN(addr, align) ((uintptr_t)(addr) & ~((uintptr_t)((align) - 1)))

#include "kheap.h"

#endif /* !MM_H */