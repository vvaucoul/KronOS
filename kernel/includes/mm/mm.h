/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 16:41:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 19:08:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MM_H
#define MM_H

#define PAGE_SIZE 4096

// #define USER_SPACE_START 0x40000000		  // 1GB
// #define USER_SPACE_END 0xC0000000		  // 3GB
// #define USER_SPACE_SIZE 0x80000000		  // 2GB
// #define USER_SPACE_INITIAL_SIZE 0x1000000 // 16MB initial size

#define KERNEL_SPACE_START 0xC0000000			  // 3GB
#define KERNEL_SPACE_END (0xFFFFF000 - 0x1000000) // ~4GB, slightly less to stay within 32-bit limit - KERNEL_SPACE_INITIAL_SIZE (16MB)s
#define KERNEL_SPACE_SIZE 0x3FFF0000			  // ~1GB

#define KERNEL_STACK_SIZE 0x4000 // 16KB

#include "kheap.h"

#endif /* !MM_H */