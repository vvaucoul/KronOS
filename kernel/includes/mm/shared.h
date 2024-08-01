/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 12:21:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 00:18:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_H
#define SHARED_H

#include <mm/memory.h>

typedef struct shared_heap_header {
    uint32_t ref_count;   // Number of processes sharing this memory
    // heap_header_t header; // Existing header
} shared_heap_header_t;

extern void *kmalloc_shared(uint32_t size);
extern void kfree_shared(void *ptr);
extern void *kdup_shared(void *ptr);

#endif /* !SHARED_H */