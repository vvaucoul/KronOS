/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dma.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 10:09:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 10:39:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DMA_H
#define DMA_H

#include <kernel.h>

/**
 * @file dma.h
 * @brief Header file for DMA (Direct Memory Access) functionality.
 */

typedef struct s_dma_driver {
    void *src;
    void *dst;
    uint32_t size;
} DmaDriver;

extern int dma_init(void);

#endif /* !DMA_H */