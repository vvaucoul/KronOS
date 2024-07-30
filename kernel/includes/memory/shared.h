/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 12:21:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 14:22:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_H
#define SHARED_H

#include <memory/memory.h>

typedef struct shared_heap_header {
    uint32_t ref_count;   // Number of processes sharing this memory
    // heap_header_t header; // Existing header
} shared_heap_header_t;

extern void *kmalloc_shared(uint32_t size);
extern void kfree_shared(void *ptr);
extern void *kdup_shared(void *ptr);

#endif /* !SHARED_H */